#include <unistd.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include "libavutil/samplefmt.h"
#include "libswresample/swresample.h"

static int open_input_file(const char *filename,AVFormatContext **ifmt_ctx)
{
    int ret;
    unsigned int i;
    ret = avformat_open_input(ifmt_ctx, filename, NULL, NULL);
    if(ret < 0)
    {
        av_log(NULL,AV_LOG_ERROR,"Cannot open input file[%s]\n",filename);
        return -1;
    }

    ret = avformat_find_stream_info(*ifmt_ctx, NULL);
    if(ret < 0)
    {
        av_log(NULL,AV_LOG_ERROR,"Cannot find stream information \n");
        return -1;
    }

    for(i = 0; i < (*ifmt_ctx)->nb_streams; i++)
    {
        AVStream *stream;
        AVCodecContext *codec_ctx;
        stream = (*ifmt_ctx)->streams[i];
        codec_ctx = stream->codec;
        if(codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO || codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            ret = avcodec_open2(codec_ctx, avcodec_find_decoder(codec_ctx->codec_id), NULL);
            if(ret < 0 )
            {
                av_log(NULL,AV_LOG_ERROR,"Failed to open decoder for stream #%u\n",i);
                return ret;
            }
        }
    }
    return 0;
}

int init_resampler(AVCodecContext *input_codec_context,AVCodecContext  **output_codec_context,SwrContext **resample_context)
{
    int ret;
    *output_codec_context = avformat_alloc_context();
    if(ret < 0)
    {
        av_log(NULL,AV_LOG_QUIET,"Could not allocate output format context\n");
        return ret;
    }

    //init output codec context
    (*output_codec_context)->sample_fmt = AV_SAMPLE_FMT_FLTP;
    (*output_codec_context)->bit_rate = 128000;
    (*output_codec_context)->sample_rate= 64000;
    (*output_codec_context)->channels= 2;
    (*output_codec_context)->channel_layout= av_get_default_channel_layout((*output_codec_context)->channels);

    //init resample context
    *resample_context = swr_alloc_set_opts(NULL,
                        (*output_codec_context)->channel_layout,
                        (*output_codec_context)->sample_fmt,
                        (*output_codec_context)->sample_rate,
                        av_get_default_channel_layout(input_codec_context->channels),
                        input_codec_context->sample_fmt,
                        input_codec_context->sample_rate, 0, NULL);

    if(!*resample_context)
    {
        av_log(NULL,AV_LOG_QUIET,"Could not allocate resample context\n");
        return -1;
    }

    ret = swr_init(*resample_context);
    if(ret < 0)
    {
        av_log(NULL,AV_LOG_QUIET,"Could not open resample context\n");
        swr_free(resample_context);
        return ret;
    }

    return 0;
}

static int init_converted_samples(uint8_t ***converted_samples, AVCodecContext *output_codec_context, int frame_size)
{
    int error;
    *converted_samples = calloc(output_codec_context->channels, sizeof(**converted_samples));
    if(!(*converted_samples))
    {
        av_log(NULL,AV_LOG_QUIET,"Could not allocate coverter input sample opinters \n");
        return AVERROR(ENOMEM);
    }

    error = av_samples_alloc(*converted_samples, NULL,
                            output_codec_context->channels,
                            frame_size,
                            output_codec_context->sample_fmt, 0);
    if(error < 0)
    {
        av_log(NULL,AV_LOG_QUIET,"Could not allocate converted input samples\n");
        av_freep(&(*converted_samples)[0]);
        free(*converted_samples);
        return error;
    }
    return 0;
}

static int convert_samples(const uint8_t **input_data, uint8_t **converted_data, const int frame_size, SwrContext *resample_context)
{
    int ret;
    ret = swr_convert(resample_context, converted_data, frame_size, input_data, frame_size);
    return ret > 0 ? ret : 0;
}

int main(int argc, char **argv)
{
    av_register_all();
    avfilter_register_all();
    if(argc != 2)
    {
        av_log(NULL,AV_LOG_QUIET,"use it like: %s test.aac\n",argv[0]);
        return -1;
    }
    int ret, i, stream_index,got_frame;
    enum AVMediaType type;
    char *inputFileName = argv[1];
    AVCodecContext *input_codec_context = NULL;
    AVCodecContext *output_codec_context = NULL;
    SwrContext *resample_context = NULL;
    uint8_t **converted_samples = NULL;
    FILE *fp = fopen("/tmp/test.pcm","wb");
    if(fp == NULL)
    {
        av_log(NULL,AV_LOG_QUIET,"Open pcm file failed\n");
        return -1;
    }
    AVFormatContext *ifmt_ctx = NULL;
    AVPacket packet = {.data = NULL, .size = 0};
    AVFrame *frame = av_frame_alloc();
    if(frame == NULL)
    {
        av_log(NULL,AV_LOG_QUIET,"allocate frame failed\n");
        goto end;
    }
    ret = open_input_file(inputFileName ,&ifmt_ctx);
    if(ret < 0)
    {
        av_log(NULL,AV_LOG_QUIET,"Open input file failed\n");
        goto end;
    }
    av_dump_format(ifmt_ctx, 0, inputFileName, 0);

    input_codec_context = ifmt_ctx->streams[0]->codec;
    init_resampler(input_codec_context, &output_codec_context, &resample_context);

    while(1){
        ret = av_read_frame(ifmt_ctx, &packet);
        if(ret < 0){
            av_log(NULL,AV_LOG_INFO,"Reached end point\n");
            break;
        }
        stream_index = packet.stream_index;
        type = ifmt_ctx->streams[stream_index]->codec->codec_type;
        av_packet_rescale_ts(&packet,
                            ifmt_ctx->streams[stream_index]->time_base,
                            ifmt_ctx->streams[stream_index]->codec->time_base);

        ret = avcodec_decode_audio4(ifmt_ctx->streams[stream_index]->codec, frame, &got_frame, &packet);
        if( ret < 0 )
        {
            av_log(NULL,AV_LOG_QUIET,"Decoding failed\n");
            break;
        }

        if(got_frame)
        {
            init_converted_samples(&converted_samples, output_codec_context, frame->nb_samples);
            ret = convert_samples((const uint8_t**)frame->extended_data, converted_samples, frame->nb_samples, resample_context);
            if(ret < 0)
            {
                av_log(NULL,AV_LOG_QUIET,"Convert failed\n");
            }
            else{
                int i, ch;
                int data_size = av_get_bytes_per_sample(output_codec_context->sample_fmt);
                printf("frame->nb_samples=%d\n",frame->nb_samples);
                printf("data_size=%d\n",data_size);
                printf("channels=%d\n",output_codec_context->channels);
                //fwrite(converted_samples[0], 1, ret, fp);

                //for(i = 0; i < frame->nb_samples; i++)
                if(ret != 1024)
                {
                    printf("ret==== %d\n",ret);
                }
                for(i = 0; i < ret; i++)
                {
                    for(ch = 0 ;ch < output_codec_context->channels; ch++)
                        fwrite(converted_samples[ch] + data_size*i, 1, data_size , fp);
                }

                //判断是否有缓存
                int fifo_size = swr_get_out_samples(resample_context, 0);
                printf("fifo_size=%d\n",fifo_size);
                //if(fifo_size > frame->nb_samples)
                if(fifo_size > 0)
                {
                    ret = swr_convert(resample_context, converted_samples,frame->nb_samples,NULL,0);
                    printf("ret=%d\n",ret);
                    for(i = 0; i < ret; i++)
                    {
                        for(ch = 0 ;ch < output_codec_context->channels; ch++)
                            fwrite(converted_samples[ch] + data_size*i, 1, data_size , fp);
                    }
                }
            };

            if(converted_samples)
            {
                av_freep(&converted_samples[0]);
                free(converted_samples);
            }
        }
        av_packet_unref(&packet);
    }
end:
    av_packet_unref(&packet);
    //av_frame_free(&frame);
    for(i = 0; i < ifmt_ctx->nb_streams; i++)
    {
        avcodec_close(ifmt_ctx->streams[i]->codec);
    }
    avformat_close_input(&ifmt_ctx);

    fclose(fp);
    return 0;
}
