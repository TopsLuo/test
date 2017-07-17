#include <stdio.h>  
#include <stdlib.h>  
#include <sys/time.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libavutil/timestamp.h>

#define USE_FILTER 1

#if USE_FILTER
//const char *filter_descr = "scale=320x240,fps=15";
char filter_descr[256] = {0};
AVFilterContext *buffersink_ctx;
AVFilterContext *buffersrc_ctx;
AVFilterGraph *filter_graph;
#endif

AVFormatContext *ifmt_ctx = NULL;
AVFormatContext *ofmt_ctx = NULL;
int video_index = 0;
int audio_index = 0;
uint64_t v_pts = 0;
uint64_t v_num = 0;
uint64_t a_num = 0;

int dst_width = 0;
int dst_height = 0;
double dst_fps = 0.0;

void init_global_params()
{
#if USE_FILTER
    dst_width = 720;
    dst_height = 576;
    dst_fps = 25;
#else
    dst_width = ifmt_ctx->streams[video_index]->codec->width;
    dst_height = ifmt_ctx->streams[video_index]->codec->height;
    dst_fps = ifmt_ctx->streams[video_index]->avg_frame_rate.num / ifmt_ctx->streams[video_index]->avg_frame_rate.den;
#endif
    printf("width=%d,height=%d,fps=%f\n",dst_width, dst_height, dst_fps);
}

#if USE_FILTER
int init_filters(const char *filters_descr)
{
    char args[512];
    int ret = 0;
    AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    AVRational time_base = ifmt_ctx->streams[video_index]->time_base;
    AVCodecContext *dec_ctx = ifmt_ctx->streams[video_index]->codec;
    //enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE };
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };

    filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
            "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
            dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
            time_base.num, time_base.den,
            dec_ctx->sample_aspect_ratio.num, dec_ctx->sample_aspect_ratio.den);

    printf("args=%s\n",args);
    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
        goto end;
    }

    /* buffer video sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, NULL, filter_graph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
        goto end;
    }

    ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
        goto end;
    }

    /*
     * Set the endpoints for the filter graph. The filter_graph will
     * be linked to the graph described by filters_descr.
     */

    /*
     * The buffer source output must be connected to the input pad of
     * the first filter described by filters_descr; since the first
     * filter input label is not specified, it is set to "in" by
     * default.
     */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    /*
     * The buffer sink input must be connected to the output pad of
     * the last filter described by filters_descr; since the last
     * filter output label is not specified, it is set to "out" by
     * default.
     */
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
                                    &inputs, &outputs, NULL)) < 0)
        goto end;

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        goto end;

end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
}
#endif

int open_input_file(const char *filename)
{
    int ret;
    if(avformat_open_input(&ifmt_ctx, filename,NULL,NULL) < 0)
    {
        printf("Open source file failed\n");
        return 0;
    }


    if(avformat_find_stream_info(ifmt_ctx, NULL) < 0)
    {
        printf("Cannot found stream info\n");
        return 0;
    }

    for(int i=0; i < ifmt_ctx->nb_streams; i++)
    {
        AVStream *stream;
        AVCodecContext *codec_ctx;
        stream = ifmt_ctx->streams[i];
        codec_ctx = stream->codec;
        printf("AVStream: %d/%d\n",stream->time_base.num, stream->time_base.den);
        printf("AVCodecContext: %d/%d\n",codec_ctx->time_base.num, codec_ctx->time_base.den);
        if(codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO || 
            codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            ret = avcodec_open2(codec_ctx,
                    avcodec_find_decoder(codec_ctx->codec_id),NULL);
            if(ret < 0)
            {
                printf("Failed to open decoder for stream\n");
                return ret;
            }
        }
    }
    av_dump_format(ifmt_ctx, 0, filename,0);
    return 0;
}

#if 1
int VideoEncoder(AVFrame *frame)
{
    static int in = 0;
    static int out = 0;
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    int ret = 0;
    int got_frame = 0;
    AVCodecContext *avctx_v = ofmt_ctx->streams[video_index]->codec;
    ret = avcodec_encode_video2(avctx_v, &pkt, frame, &got_frame);
    if(ret < 0)
    {
        av_log(NULL,AV_LOG_ERROR,"Encode failed\n");
        return ret;
    }

    in++;

    if(got_frame)
    {
    #if 0//根据原有的时间戳做时间戳的转换
        //一定要，否则帧率会出错
        av_packet_rescale_ts(&pkt,
                            ifmt_ctx->streams[video_index]->time_base,
                            ofmt_ctx->streams[video_index]->time_base);
    #else//根据帧数转换成时间再转换成时间戳
        double time = v_num * av_q2d(ofmt_ctx->streams[video_index]->codec->time_base);
        //double time = v_num / av_q2d(ifmt_ctx->streams[video_index]->avg_frame_rate);
        pkt.pts = time / av_q2d(ofmt_ctx->streams[video_index]->time_base);
        pkt.dts = pkt.pts;
    #endif
        /*
        printf("v in codec:%d,%d\n",ifmt_ctx->streams[video_index]->codec->time_base.num,
                         ifmt_ctx->streams[video_index]->codec->time_base.den);//这个计算出来的帧率是avg_frame_rate的两倍
        printf("v in:%d,%d\n",ifmt_ctx->streams[video_index]->avg_frame_rate.num,
                         ifmt_ctx->streams[video_index]->avg_frame_rate.den);//这个很特别
        printf("v in:%d,%d\n",ifmt_ctx->streams[video_index]->time_base.num,
                         ifmt_ctx->streams[video_index]->time_base.den);
        printf("v ot:%d,%d\n",ofmt_ctx->streams[video_index]->time_base.num,
                         ofmt_ctx->streams[video_index]->time_base.den);
        */
        //printf("video:time=%f ,pts=%lld,dts=%lld,frame_pts=%lld\n",time, pkt.pts, pkt.dts,frame->pts);
        pkt.stream_index = video_index;
        av_interleaved_write_frame(ofmt_ctx, &pkt);
        v_num++;
        out++;
    }
    av_packet_unref(&pkt);
    printf("in: %d, out: %d\n",in, out);
    return ret;
}
#else
int VideoEncoder(AVFrame *frame)
{
    static int in = 0;
    static int out = 0;
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    int ret = 0;
    AVCodecContext *avctx_v = ofmt_ctx->streams[video_index]->codec;
    ret = avcodec_send_frame(avctx_v, frame);
    if(ret < 0)
    {
        av_log(NULL,AV_LOG_ERROR,"Encode failed\n");
        return ret;
    }
    in++;

    while(1)
    {
        ret = avcodec_receive_packet(avctx_v, &pkt);
        if(ret == AVERROR(EAGAIN))
            break;
        if(ret < 0)
            break;

        double time = v_num * av_q2d(ofmt_ctx->streams[video_index]->codec->time_base);
        //double time = v_num / av_q2d(ifmt_ctx->streams[video_index]->avg_frame_rate);
        pkt.pts = time / av_q2d(ofmt_ctx->streams[video_index]->time_base);
        pkt.dts = pkt.pts;

        pkt.stream_index = video_index;
        av_interleaved_write_frame(ofmt_ctx, &pkt);
        v_num++;
        out++;
        av_packet_unref(&pkt);
    }
    printf("in: %d, out: %d\n",in, out);
    return ret;
}
#endif

int AudioEncoder(AVFrame *frame)
{
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    int ret = 0;
    int got_frame = 0;
    AVCodecContext *avctx_a = ofmt_ctx->streams[audio_index]->codec;
    ret = avcodec_encode_audio2(avctx_a, &pkt, frame, &got_frame);
    if(ret < 0)
    {
        av_log(NULL,AV_LOG_ERROR,"Encode failed\n");
        return ret;
    }
    if(got_frame)
    {
    #if 0
        av_packet_rescale_ts(&pkt,
                            ifmt_ctx->streams[audio_index]->time_base,
                            ofmt_ctx->streams[audio_index]->time_base);
    #else
        double time = a_num * 1024 * av_q2d(ifmt_ctx->streams[audio_index]->codec->time_base);
        pkt.pts = time / av_q2d(ofmt_ctx->streams[audio_index]->time_base);
        pkt.dts = pkt.pts;
    #endif
        /*
        printf("a in codec :%d,%d\n",ifmt_ctx->streams[audio_index]->codec->time_base.num,
                         ifmt_ctx->streams[audio_index]->codec->time_base.den);
        printf("a in avg_frame_rate:%d,%d\n",ifmt_ctx->streams[audio_index]->avg_frame_rate.num,
                         ifmt_ctx->streams[audio_index]->avg_frame_rate.den);
        printf("a in sample_rate:%d,%d\n",ifmt_ctx->streams[audio_index]->avg_frame_rate.num,
                         ifmt_ctx->streams[audio_index]->avg_frame_rate.den);
        printf("a in ifmt_time_base:%d,%d\n",ifmt_ctx->streams[audio_index]->time_base.num,
                         ifmt_ctx->streams[audio_index]->time_base.den);
        printf("a ot ofmt_time_base:%d,%d\n",ofmt_ctx->streams[audio_index]->time_base.num,
                         ofmt_ctx->streams[audio_index]->time_base.den);
        */
        //printf("audio:pts=%lld,dts=%lld,frame_pts=%lld\n",pkt.pts, pkt.dts,frame->pts);
        pkt.stream_index = audio_index;
        av_interleaved_write_frame(ofmt_ctx, &pkt);
        a_num++;
    }
    av_packet_unref(&pkt);
    return ret;
}

int open_output_file(const char *filename)
{
    int ret,i;
    AVStream *out_stream;
    AVStream *in_stream;
    AVCodecContext *dec_ctx, *enc_ctx;
    AVCodec *encoder;

    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, filename);
    if(!ofmt_ctx)
    {
        printf("Cold not create output context\n");
        return AVERROR_UNKNOWN;
    }

    for(i = 0; i < ifmt_ctx->nb_streams; i++)
    {
        out_stream = avformat_new_stream(ofmt_ctx, NULL);
        if(!out_stream)
        {
            printf("Failed allocating output stream\n");
            return AVERROR_UNKNOWN;
        }
        in_stream = ifmt_ctx->streams[i];
        dec_ctx = in_stream->codec;
        enc_ctx = out_stream->codec;
        
        if(dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_index = i;
            encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
            //encoder = avcodec_find_encoder(dec_ctx->codec_id);
            if(!encoder)
            {
                printf("Necessary video encoder not found\n");
                return AVERROR_UNKNOWN;
            }
            enc_ctx->width= dst_width;//dec_ctx->width;
            enc_ctx->height = dst_height;//dec_ctx->height;
            //enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;
            enc_ctx->pix_fmt = dec_ctx->pix_fmt;
            //av_log(NULL,AV_LOG_QUIET,"tbc: %d/%d\n", enc_ctx->time_base.den,enc_ctx->time_base.num);
            //av_log(NULL,AV_LOG_QUIET,"tbn: %d/%d\n", in_stream->time_base.den, in_stream->time_base.num);
            //av_log(NULL,AV_LOG_QUIET,"fps: %d/%d\n", in_stream->avg_frame_rate.den, in_stream->avg_frame_rate.num);
            //av_log(NULL,AV_LOG_QUIET,"tbr: %d/%d\n", in_stream->r_frame_rate.den, in_stream->r_frame_rate.num);

            //enc_ctx->time_base = dec_ctx->time_base;
            enc_ctx->time_base = (AVRational){1,dst_fps};
            enc_ctx->max_qdiff = 4;
            enc_ctx->qmin = 10;
            enc_ctx->qmax = 51;
            enc_ctx->has_b_frames = 0;
            enc_ctx->max_b_frames = 0;
            enc_ctx->bit_rate = 1000000;
            enc_ctx->thread_count = 4;
        }
        else if(dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audio_index = i;
            encoder = avcodec_find_encoder(AV_CODEC_ID_AAC);
            if(!encoder)
            {
                printf("Necessary audio encoder not found\n");
                return AVERROR_UNKNOWN;
            }
            enc_ctx->sample_rate = dec_ctx->sample_rate;
            enc_ctx->channel_layout = dec_ctx->channel_layout;
            enc_ctx->channels = av_get_channel_layout_nb_channels(enc_ctx->channel_layout);
            enc_ctx->sample_fmt = AV_SAMPLE_FMT_FLTP;
            enc_ctx->time_base = (AVRational){1,enc_ctx->sample_rate};
            av_opt_set(enc_ctx->priv_data, "preset","ultrafast", 0);
            //av_opt_set(enc_ctx->priv_data, "tune","zerolatency", 0);
            //enc_ctx->time_base = (AVRational){1,90000};
        }
        else{
            printf("UNknow stream type\n");
            continue;
        }

        //mp4文件的重点在此
        //创建完h264编码器后，enc_ctx中的extradata并没有数据，只有当调用avcodec_open2的时候
        //。都会把sps pps数据写入extradata中去，但写入也有条件，必须设置CODEC_FLAG_GLOBAL_HEADER，
        //所以avcodec_open2要在设置好GLOBAL_HEADER之后在调用，否则会出现不报错，一切正常，但
        //就是不能正常播放
        if(ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        {
            enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            printf("need global header\n");
        }

        ret = avcodec_open2(enc_ctx, encoder, NULL);
        if(ret < 0)
        {
            printf("Cannot open encoder for  stream #%u\n",i);
            return ret;
        }

        ret = avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);
        if(ret < 0)
        {
            printf("Could not initialize stream parameters");
            return ret;
        }
    }
    av_dump_format(ofmt_ctx, 0, filename, 1);

    if(!(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
    {
        ret = avio_open(&ofmt_ctx->pb, filename ,AVIO_FLAG_WRITE);
        if(ret < 0)
        {
            printf("Could not open output file %s\n",filename);
            return ret;
        }
    }

    AVDictionary *opt = NULL;
    //av_dict_set_int(&opt, "video_track_timescale" ,"25", 0);
    ret = avformat_write_header(ofmt_ctx,&opt);
    if(ret < 0)
    {
        printf("Error occurred when opening output file\n");
        return ret;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int ret;
    int64_t start_time = 0;
    int is_first_frame = 1;
    av_register_all();
    avfilter_register_all();
    int (*dec_func)(AVCodecContext *, AVFrame *, int *, const AVPacket *);
    if(argc != 3)
    {
        printf("Please input params,use this program like:\n");
        printf("%s  <input_file>  <output_file>\n",argv[0]);
        return 0;
    }

    char *src_file = argv[1];
    char *dst_file = argv[2];
    if(open_input_file(src_file) < 0)
    {
        printf("Could not open input file\n");
        return 0;
    }
    init_global_params();

#if USE_FILTER
    //测试发现，start_time的意思是从当前文件的多少秒开始，简言之就是跳过片头多少秒的意思
    //sprintf(filter_descr,"scale=%dx%d,fps=fps=%f:start_time=30.",dst_width, dst_height,dst_fps);
    sprintf(filter_descr,"scale=%dx%d,fps=fps=%f",dst_width, dst_height,dst_fps);
    printf("filter_descr=%s\n",filter_descr);
    if(init_filters(filter_descr) < 0)
    {
        printf("Could not init video filter\n");
        return 0;
    }
#endif

    if(open_output_file(dst_file) < 0)
    {
        printf("Could not open output file\n");
        return 0; 
    }

    printf("video_index=%d,audio_index=%d\n",video_index, audio_index);
    AVPacket packet = {.data = NULL, .size = 0};
    enum AVMediaType type;
    int got_frame = 0;
    int stream_index;
    AVFrame *frame = av_frame_alloc();
    AVFrame *filt_frame = av_frame_alloc();
    if(frame == NULL || filt_frame == NULL)
    {
        av_log(NULL,AV_LOG_ERROR,"alloc frame failed\n");
        exit(0);
    }

    while(1)
    {
        ret = av_read_frame(ifmt_ctx, &packet);
        if(ret < 0)
        {
            av_log(NULL,AV_LOG_ERROR,"Read end\n");
            break;
        }

        stream_index = packet.stream_index;
        type = ifmt_ctx->streams[stream_index]->codec->codec_type;
        if(type == AVMEDIA_TYPE_VIDEO && is_first_frame)
        {
            is_first_frame = 0;
            start_time = packet.pts;
        }
        /*printf("orig decodec pkt.pts=%lld, pkt.dts=%lld\n",packet.pts,packet.dts);
        av_packet_rescale_ts(&packet,
                            ifmt_ctx->streams[stream_index]->time_base,
                            ofmt_ctx->streams[stream_index]->time_base);
        printf("time_base:num=%d,den=%d\n",ifmt_ctx->streams[stream_index]->time_base.num,ifmt_ctx->streams[stream_index]->time_base.den);
        printf("==time_base:num=%d,den=%d\n",ifmt_ctx->streams[stream_index]->codec->time_base.num,ifmt_ctx->streams[stream_index]->codec->time_base.den);*/
        dec_func = (type == AVMEDIA_TYPE_VIDEO) ? avcodec_decode_video2 : avcodec_decode_audio4;
        ret = dec_func(ifmt_ctx->streams[stream_index]->codec, frame, &got_frame, &packet);
        if(ret < 0)
        {
            av_log(NULL,AV_LOG_ERROR,"Decoding failed\n");
            break;
        }
        if(got_frame)
        {
            if(type == AVMEDIA_TYPE_VIDEO)
            {
            #if !USE_FILTER
                //printf("v orig pkt.pts=%lld, pkt.dts=%lld\n",packet.pts,packet.dts);
                VideoEncoder(frame);
                double time = (frame->pts - start_time) * av_q2d(ifmt_ctx->streams[video_index]->time_base);
                double delta = time * 23.976;
                //av_log(NULL,AV_LOG_QUIET,"pts_time: %f, delta: %f\n",time, delta );
            #else
                frame->pts = av_frame_get_best_effort_timestamp(frame);
                if(av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0)
                {
                    av_log(NULL,AV_LOG_ERROR,"Error while feeding the filtergraph\n");
                    break;
                }

                while(1)
                {
                    ret = av_buffersink_get_frame(buffersink_ctx, filt_frame);
                    if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                        break;
                    if(ret < 0)
                        break;

                    filt_frame->pict_type = AV_PICTURE_TYPE_NONE;
                    //av_log(NULL,AV_LOG_QUIET,"filt_frame pts: %"PRId64"\n",filt_frame->pts);
                    VideoEncoder(filt_frame);
                    av_frame_unref(filt_frame);
                }
            #endif
            }
            else if(type == AVMEDIA_TYPE_AUDIO)
            {
                //printf("a orig pkt.pts=%lld, pkt.dts=%lld\n",packet.pts,packet.dts);
                AudioEncoder(frame);
            }
        }

        av_packet_unref(&packet);
        av_frame_unref(frame);
    }
    av_write_trailer(ofmt_ctx);

    av_frame_free(&frame);
    av_frame_free(&filt_frame);
    for(int i = 0; i < ifmt_ctx->nb_streams; i++)
    {
        avcodec_close(ifmt_ctx->streams[i]->codec);
        if(ofmt_ctx && ofmt_ctx->nb_streams > i && ofmt_ctx->streams[i] && ofmt_ctx->streams[i]->codec)
            avcodec_close(ofmt_ctx->streams[i]->codec);
    }

    avformat_close_input(&ifmt_ctx);
    if(ofmt_ctx && !(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&ofmt_ctx->pb);

    avformat_free_context(ofmt_ctx);
#if USE_FILTER
    avfilter_graph_free(&filter_graph);
#endif

    return 0;
}
