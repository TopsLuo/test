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
#include "libavutil/mathematics.h"

int main(int argc, char *argv[])
{
    AVOutputFormat *ofmt = NULL;
    AVFormatContext *ifmt_ctx_v = NULL, *ifmt_ctx_a = NULL, *ofmt_ctx = NULL;
    AVPacket pkt;
    int ret, i;
    int videoindex_in = -1,videoindex_out = -1;
    int audioindex_in = -1,audioindex_out = -1;
    int video_frame_index = 0;
    int audio_frame_index = 0;
    int64_t cur_pts_v = 0, cur_pts_a = 0;
    const char *in_video_filename = "test.264";
    const char *in_audio_filename = "test.aac";
    const char *out_filename = "test.mp4";

    av_register_all();
    //读取视频文件
    ret = avformat_open_input(&ifmt_ctx_v, in_video_filename,0,0);
    ret = avformat_find_stream_info(ifmt_ctx_v, 0);

    //读取音频文件
    ret = avformat_open_input(&ifmt_ctx_a, in_audio_filename,0,0);
    ret = avformat_find_stream_info(ifmt_ctx_a, 0);

    av_dump_format(ifmt_ctx_v, 0, in_video_filename, 0);
    av_dump_format(ifmt_ctx_a, 0, in_audio_filename, 0);

    //创建输出文件
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
    ofmt = ofmt_ctx->oformat;
    //在创建的输出文件中添加视频流
    for(i = 0; i < ifmt_ctx_v->nb_streams;i++)
    {
        AVStream *in_stream = ifmt_ctx_v->streams[i];
        AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
        videoindex_in = i;
        videoindex_out = out_stream->index;
        avcodec_copy_context(out_stream->codec, in_stream->codec);
        out_stream->codec->codec_tag = 0;
        if(ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
        break;
    }

    //在创建的输出文件中添加音频流
    for(i = 0; i < ifmt_ctx_a->nb_streams; i++)
    {
        AVStream *in_stream = ifmt_ctx_a->streams[i];
        AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
        audioindex_in = i;
        audioindex_out = out_stream->index;
        avcodec_copy_context(out_stream->codec, in_stream->codec);
        out_stream->codec->codec_tag = 0;
        if(ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
        break;
    }
    av_dump_format(ofmt_ctx, 0, out_filename, 1);

    if( !(ofmt->flags & AVFMT_NOFILE) )
    {
        avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
    }

    AVDictionary *opt = NULL;
    av_dict_set_int(&opt, "video_track_timescale",25,0);
    avformat_write_header(ofmt_ctx, &opt);

    while(1)
    {
        AVFormatContext *ifmt_ctx;
        int stream_index = 0;
        AVStream *in_stream, *out_stream;
        if(av_compare_ts(cur_pts_v, ifmt_ctx_v->streams[videoindex_in]->time_base, cur_pts_a, ifmt_ctx_a->streams[audioindex_in]->time_base) < 0)
        {
            ifmt_ctx = ifmt_ctx_v;
            stream_index = videoindex_out;
            if(av_read_frame(ifmt_ctx, &pkt) >= 0)
            {
                do{
                    in_stream = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = ofmt_ctx->streams[stream_index];
                    //printf("==============================\n");
                    //printf("video in_stream  den=%d,num=%d\n",in_stream->time_base.den, in_stream->time_base.num);
                    //printf("video out_stream den=%d,num=%d\n",out_stream->time_base.den, out_stream->time_base.num);
                    //printf("video in_r_frame_rate den=%d,num=%d\n",in_stream->r_frame_rate.den, in_stream->r_frame_rate.num);
                    if(pkt.stream_index == videoindex_in)
                    {
                        if(pkt.pts == AV_NOPTS_VALUE)
                        {
                            AVRational time_base1 = in_stream->time_base;
                            int64_t calc_duration = (double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);
                            pkt.pts = (double)(video_frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            pkt.dts = pkt.pts;
                            //printf("orig v pts=%lld,dts=%lld\n",pkt.pts, pkt.dts);
                            pkt.duration = (double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                        }
                        cur_pts_v = pkt.pts;
                        video_frame_index++;
                        break;
                    }
                }while(av_read_frame(ifmt_ctx, &pkt) >=0);
            }
            else
            {
                break;
            }
        }
        else
        {
            ifmt_ctx = ifmt_ctx_a;
            stream_index = audioindex_out;
            if(av_read_frame(ifmt_ctx, &pkt) >=0)
            {
                do{
                    in_stream = ifmt_ctx->streams[pkt.stream_index];
                    out_stream =ofmt_ctx->streams[stream_index];
                    //printf("------------------------------\n");
                    printf("audio out_stream den=%d, num=%d\n",out_stream->time_base.den,out_stream->time_base.num);
                    printf("audio in_stream den=%d, num=%d\n",in_stream->time_base.den,in_stream->time_base.num);
                    //printf("audio in_r_frame_rate den=%d,num=%d\n",in_stream->sample_aspect_ratio.den, in_stream->sample_aspect_ratio.num);
                    if(pkt.stream_index == audioindex_in)
                    {
                        if(pkt.pts == AV_NOPTS_VALUE)
                        {
                            AVRational time_base1 = in_stream->time_base;
                            int64_t calc_duration = (double)AV_TIME_BASE/av_q2d(in_stream->sample_aspect_ratio);
                            pkt.pts = (double)(audio_frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            pkt.dts = pkt.pts;
                            pkt.duration = (double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                        }
                        cur_pts_a = pkt.pts;
                        audio_frame_index++;
                        break;
                    }
                }while(av_read_frame(ifmt_ctx, &pkt) >= 0);
            }
            else
            {
                break;
            }
        }


        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        if(stream_index == videoindex_out)
        {
            //printf("den=%d, num=%d\n",out_stream->time_base.den,out_stream->time_base.num);
            //printf("v pts=%lld,dts=%lld\n",pkt.pts, pkt.dts);
        }
        else
        {
            printf("a pts=%lld,dts=%lld\n",pkt.pts, pkt.dts);
        }
        pkt.stream_index =stream_index;
        av_interleaved_write_frame(ofmt_ctx, &pkt);
        av_free_packet(&pkt);
    }
    av_write_trailer(ofmt_ctx);

    avformat_close_input(&ifmt_ctx_v);
    avformat_close_input(&ifmt_ctx_a);

    if(ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_close(ofmt_ctx->pb);

    avformat_free_context(ofmt_ctx);
    return 0;
}
