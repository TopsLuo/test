#include <unistd.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfiltergraph.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavutil/opt.h"
#include "libavutil/audio_fifo.h"


static AVFormatContext *fmt_ctx;
static AVCodecContext *dec_ctx;
static int audio_stream_index = -1;

AVFilterContext *buffersink_ctx;
AVFilterContext *buffersrc_ctx;
AVFilterGraph *filter_graph;
static const char *filter_descr = "aresample=64000:async=1024,aformat=sample_fmts=fltp:channel_layouts=stereo";
//static const char *filter_descr = "aresample=async=512:sample_rate=64000,aformat=sample_fmts=fltp:channel_layouts=stereo";
static FILE *fp = NULL;

AVFrame *alloc_audio_frame(const int channels, const int64_t layout, enum AVSampleFormat sample_fmt, int align)
{
    AVFrame *frame;
    int buffer_size;
    uint16_t *samples;
    frame = av_frame_alloc();
    if(!frame)
    {
        av_log(NULL,AV_LOG_QUIET,"Could not allocate frame\n");
        return NULL;
    }

    frame->nb_samples = 1024;
    frame->format = sample_fmt;
    frame->channel_layout = av_get_default_channel_layout(channels);

    buffer_size = av_samples_get_buffer_size(NULL, channels, frame->nb_samples, frame->format ,0);

    if(buffer_size < 0)
    {
        av_log(NULL,AV_LOG_QUIET,"Could not allocate buffer size\n");
        return NULL;
    }

    samples = av_malloc(buffer_size);
    if(!samples)
    {
        av_log(NULL,AV_LOG_QUIET,"Could not allocate samples\n");
        return NULL;
    }

    if(avcodec_fill_audio_frame(frame, channels, frame->format,(const uint16_t*)samples, buffer_size,0) < 0)
    {
        av_log(NULL,AV_LOG_QUIET,"Could not setup audio frame\n");
        return NULL;
    }

    return frame;
}

static void write_frame(const AVFrame *frame)
{
    int i, ch;
    int data_size = av_get_bytes_per_sample( AV_SAMPLE_FMT_FLTP);
    int channels = av_get_channel_layout_nb_channels( AV_CH_LAYOUT_STEREO);
    for(i=0;i < frame->nb_samples; i++)
    {
        for(ch = 0; ch < channels; ch++)
        {
            fwrite(frame->data[ch] + data_size*i, 1, data_size, fp);
        }
    }
}

static int open_input_file(const char *filename)
{
    int ret;
    AVCodec *dec = NULL;
    ret = avformat_open_input(&fmt_ctx, filename ,NULL, NULL);
    if(ret < 0)
    {
        av_log(NULL,AV_LOG_QUIET,"Cannot open input file\n");
        return ret;
    }

    ret = avformat_find_stream_info(fmt_ctx, NULL);
    if(ret < 0)
    {
        av_log(NULL,AV_LOG_QUIET,"Cannot find stream information\n");
        return ret;
    }

    //select the audio stream
    ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
    if(ret < 0)
    {
        av_log(NULL,AV_LOG_QUIET,"Cannot find an audio stream in the input file\n");
        return ret;
    }
    audio_stream_index = ret;
    dec_ctx = fmt_ctx->streams[audio_stream_index]->codec;
    av_opt_set_int(dec_ctx, "recounted_frames", 1, 0);

    ret = avcodec_open2(dec_ctx, dec, NULL);
    if(ret < 0)
    {
        av_log(NULL,AV_LOG_QUIET,"Cannot open audio decoder\n");
        return ret;
    }

    return 0;
}

static int init_filters(const char *filters_descr)
{
    char args[512];
    int ret = 0;
    AVFilter *abuffersrc  = avfilter_get_by_name("abuffer");
    AVFilter *abuffersink = avfilter_get_by_name("abuffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    static const enum AVSampleFormat out_sample_fmts[] = { AV_SAMPLE_FMT_FLTP, -1 };
    static const int64_t out_channel_layouts[] = { AV_CH_LAYOUT_STEREO, -1 };
    static const int out_sample_rates[] = { 64000, -1 };
    const AVFilterLink *outlink;
    AVRational time_base = fmt_ctx->streams[audio_stream_index]->time_base;

    filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /* buffer audio source: the decoded frames from the decoder will be inserted here. */
    if (!dec_ctx->channel_layout)
        dec_ctx->channel_layout = av_get_default_channel_layout(dec_ctx->channels);
    snprintf(args, sizeof(args),
            "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%"PRIx64,
             time_base.num, time_base.den, dec_ctx->sample_rate,
             av_get_sample_fmt_name(dec_ctx->sample_fmt), dec_ctx->channel_layout);
    printf("%s\n",args);
    ret = avfilter_graph_create_filter(&buffersrc_ctx, abuffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer source\n");
        goto end;
    }

    /* buffer audio sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&buffersink_ctx, abuffersink, "out",
                                       NULL, NULL, filter_graph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer sink\n");
        goto end;
    }

    ret = av_opt_set_int_list(buffersink_ctx, "sample_fmts", out_sample_fmts, -1,
                              AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot set output sample format\n");
        goto end;
    }

    ret = av_opt_set_int_list(buffersink_ctx, "channel_layouts", out_channel_layouts, -1,
                              AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot set output channel layout\n");
        goto end;
    }

    ret = av_opt_set_int_list(buffersink_ctx, "sample_rates", out_sample_rates, -1,
                              AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot set output sample rate\n");
        goto end;
    }

    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
                                        &inputs, &outputs, NULL)) < 0)
        goto end;

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        goto end;

    /* Print summary of the sink buffer
     * Note: args buffer is reused to store channel layout string */
    outlink = buffersink_ctx->inputs[0];
    av_get_channel_layout_string(args, sizeof(args), -1, outlink->channel_layout);
    av_log(NULL, AV_LOG_INFO, "Output: srate:%dHz fmt:%s chlayout:%s\n",
           (int)outlink->sample_rate,
           (char *)av_x_if_null(av_get_sample_fmt_name(outlink->format), "?"),
           args);

end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
}

static int init_fifo(AVAudioFifo **fifo)
{
    *fifo = av_audio_fifo_alloc(AV_SAMPLE_FMT_FLTP, 2,1);
    if(!(*fifo))
    {
        av_log(NULL,AV_LOG_QUIET,"Could not aoolcate fifo\n");
        return AVERROR(ENOMEM);
    }
    return 0;
}

static int add_samples_to_fifo(AVAudioFifo *fifo, uint8_t **data, const int frame_size)
{
    int error;
    error = av_audio_fifo_realloc(fifo, av_audio_fifo_size(fifo) + frame_size);
    if(error < 0 )
    {
        return error;
    }

    if(av_audio_fifo_write(fifo, (void **)data, frame_size) < frame_size)
    {
        av_log(NULL,AV_LOG_QUIET,"Could not write data to fifo\n");
        return AVERROR_EXIT;
    }
    return 0;
}

static int init_output_frame(AVFrame **frame, int frame_size)
{
    *frame = av_frame_alloc();
    (*frame)->nb_samples = frame_size;
    (*frame)->channel_layout = AV_CH_LAYOUT_STEREO;
    (*frame)->format = AV_SAMPLE_FMT_FLTP;
    (*frame)->sample_rate = 64000;

    av_frame_get_buffer(*frame, 0);
    
}

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        fprintf(stderr,"use it like this:"); 
        fprintf(stderr,"%s input_file output.pcm\n"); 
        return 0;
    }

    int ret;
    AVAudioFifo *fifo = NULL;
    AVPacket packet0, packet;
    AVFrame *frame = av_frame_alloc();
    AVFrame *filt_frame = av_frame_alloc();

    int got_frame;
    if(!frame || !filt_frame)
    {
        perror("Could not allocate frame");
        exit(1);
    }

    av_register_all();
    avfilter_register_all();

    init_fifo(&fifo);

    ret = open_input_file(argv[1]);
    if(ret < 0)
    {
        av_log(NULL,AV_LOG_QUIET,"Open input file failed\n");
        goto end;
    }

    ret = init_filters(filter_descr);
    if(ret < 0)
    {
        av_log(NULL,AV_LOG_QUIET,"init filters failed\n");
        goto end;
    }
    fp = fopen(argv[2], "wb");
    if(fp == NULL)
    {
        av_log(NULL,AV_LOG_QUIET,"open file failed\n");
        goto end;
    }

    packet0.data = NULL;
    packet.data = NULL;
    while (1) {
        if (!packet0.data) {
            if ((ret = av_read_frame(fmt_ctx, &packet)) < 0)
                break;
            packet0 = packet;
        }

        if (packet.stream_index == audio_stream_index) {
            got_frame = 0;
            ret = avcodec_decode_audio4(dec_ctx, frame, &got_frame, &packet);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Error decoding audio\n");
                continue;
            }
            packet.size -= ret;
            packet.data += ret;

            if (got_frame) {
                /* push the audio data from decoded frame into the filtergraph */
                if (av_buffersrc_add_frame_flags(buffersrc_ctx, frame, 0) < 0) {
                    av_log(NULL, AV_LOG_ERROR, "Error while feeding the audio filtergraph\n");
                    break;
                }

                /* pull filtered audio from the filtergraph */
                while (1) {
                    ret = av_buffersink_get_frame_flags(buffersink_ctx, filt_frame, 0);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                        break;
                    if (ret < 0)
                        goto end;
                    //printf("frame->nb_samples=%d\n",frame->nb_samples);
                    //printf("frame->linesize=%d\n",frame->linesize[0]);
                    //printf("filt_frame->nb_samples=%d\n",filt_frame->nb_samples);
                    //printf("filt_frame->linesize=%d\n",filt_frame->linesize[0]);
                    //write_frame(filt_frame, f);
                    //while(av_audio_fifo_size(fifo) < 1024)
                    //while(av_audio_fifo_size(fifo) < filt_frame->nb_samples)
                    {
                        add_samples_to_fifo(fifo, filt_frame->data, filt_frame->nb_samples);
                    }
                    while(av_audio_fifo_size(fifo) > 1024)
                    {
                        AVFrame *f;
                        init_output_frame(&f, 1024);
                        av_audio_fifo_read(fifo, (void **)f->data, 1024);
                        write_frame(f);
                        av_frame_free(&f);
                    }

                    av_frame_unref(filt_frame);
                }
            }

            if (packet.size <= 0)
                av_packet_unref(&packet0);
        } else {
            av_packet_unref(&packet0);
        }
    }

end:
    if(fifo)
        av_audio_fifo_free(fifo);
    avfilter_graph_free(&filter_graph);
    avcodec_close(dec_ctx);
    avformat_close_input(&fmt_ctx);
    av_frame_free(&frame);
    av_frame_free(&filt_frame);
    return 0;
}

