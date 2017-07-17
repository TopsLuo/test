/***************************************************************************
*为了测试av_frame_alloc()的BUG，写的一段代码：
*在实际开发中，发现如果直接从解码器中获取到的frame，直接存储在list中，在不删除
*list中的iterator的情况下，list的front()数据是会被改变的，最终测试发现，把frame
* clone到另外一个frame，然后push_back到list中可以解决该问题
***************************************************************************/
#include <iostream>
#include <list>
#include <map>
#include <unistd.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C"{
#endif
#include "libxml/parser.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavfilter/avfilter.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
#include "libavutil/samplefmt.h"
#include "libavutil/pixdesc.h"

#ifdef __cplusplus
}
#endif

using namespace std;
list<AVFrame*> a;
list<AVFrame*>::iterator m;
map<AVFrame*, time_t> Cache;
map<AVFrame*, time_t>::iterator map_iter;
map<uint64_t,pair<AVFrame*, time_t> > Cache_1;
map<uint64_t,pair<AVFrame*, time_t> >::iterator map_iter_1;
static int idx = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

AVFormatContext *ifmt_ctx;
FILE *fp_yuv = NULL;
static int open_input_file(const char* filename);
static void WriteDecodeVideoFrame(AVFrame *frame);

void *read_thread(void* args)
{
    while(true)
    {
        usleep(1000);
        pthread_mutex_lock(&mutex);
        if(a.empty())
        {
            pthread_mutex_unlock(&mutex);
            cout << "list is empty" <<endl;
            continue;
        }
        //cout << "begin=" << a.size()<<endl;
        //a.pop_front();
        AVFrame *fr = a.front();
        WriteDecodeVideoFrame(fr);
        for(map_iter_1 = Cache_1.begin();map_iter_1 != Cache_1.end();map_iter_1++)
        {
            //cout << map_iter_1->first << ' ' << map_iter_1->second << endl;
            cout << map_iter_1->first << ' ' << map_iter_1->second.first << ' ' << map_iter_1->second.second<<endl;
            //pair<AVFrame*,time_t> a = map_iter_1->second;
        }
        cout <<"=====================================================" << endl;

        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char **argv)
{
    av_register_all();
    pthread_t handle;
    pthread_create(&handle, NULL, read_thread, NULL);
    AVPacket packet = {0};
    av_init_packet(&packet);
    packet.data = NULL;
    packet.size = 0;
    enum AVMediaType type;
    unsigned int stream_index;
    int got_frame;
    int ret;
    AVFrame *frame = NULL;
    int (*dec_func)(AVCodecContext *, AVFrame *, int *, const AVPacket *);
    if(ret = open_input_file(argv[1]) < 0)
    {
        av_log(NULL,AV_LOG_ERROR,"Open input file failed\n");
        exit(1);
    }

    fp_yuv = fopen("list_test.yuv","wb");
    if(!fp_yuv)
    {
        av_log(NULL,AV_LOG_ERROR,"Open yuv file failed\n");
        exit(1);
    }

    frame = av_frame_alloc();
    if(frame == NULL)
    {
        av_log(NULL,AV_LOG_ERROR,"alloc frame failed\n");
        exit(0);
    }

    while(true)
    {

        ret = av_read_frame(ifmt_ctx, &packet);
        if(ret < 0)
        {
            av_log(NULL,AV_LOG_ERROR,"read end\n");
            break;
        }
        stream_index = packet.stream_index;
        type = ifmt_ctx->streams[stream_index]->codec->codec_type;
        dec_func = (type == AVMEDIA_TYPE_VIDEO) ? avcodec_decode_video2 : avcodec_decode_audio4;
        ret = dec_func(ifmt_ctx->streams[stream_index]->codec, frame, &got_frame, &packet);
        if(ret < 0)
        {
            av_log(NULL,AV_LOG_ERROR,"Decoding failed\n");
            break;
        }
        if(got_frame)
        {
            av_log(NULL,AV_LOG_ERROR,"Decoding successful\n");
            if(type == AVMEDIA_TYPE_VIDEO)
            { 
                time_t timep;
                time(&timep);
                AVFrame *f = av_frame_clone(frame);
                pthread_mutex_lock(&mutex);
                a.push_back(f);
                Cache.insert(pair<AVFrame*,time_t>(f,timep));
                pair<AVFrame*,time_t> a(f,timep);
                Cache_1.insert(map<uint64_t,pair<AVFrame*,time_t> >::value_type(idx++,a));
                pthread_mutex_unlock(&mutex);
            }
            av_packet_unref(&packet);
        }
    }

    av_frame_free(&frame);
    //pthread_join(handle, NULL);
    return 0;
}

static int open_input_file(const char* filename)
{
    int ret;
    unsigned int i;

    ifmt_ctx = NULL;
    ret = avformat_open_input(&ifmt_ctx, filename,NULL,NULL);
    if(ret < 0)
    {
        av_log(NULL,AV_LOG_ERROR,"Cannot open input filename,%s\n",filename);
        return ret;
    }

    ret = avformat_find_stream_info(ifmt_ctx, NULL);
    if(ret < 0)
    {
        av_log(NULL,AV_LOG_ERROR,"Cannot find stream information\n");
        return ret;
    }

    for(i = 0; i < ifmt_ctx->nb_streams;i++)
    {
        AVStream *stream;
        AVCodecContext *codec_ctx;
        stream = ifmt_ctx->streams[i];
        codec_ctx = stream->codec;
        if(codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO ||
        codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            ret = avcodec_open2(codec_ctx,
                    avcodec_find_decoder(codec_ctx->codec_id), NULL);
            if(ret < 0)
            {
                av_log(NULL,AV_LOG_ERROR,"failed to open decoder for stream");
                return ret;
            }
        }
    }
    
    av_dump_format(ifmt_ctx, 0, filename ,0);
}

void WriteDecodeVideoFrame(AVFrame *frame)
{
    if(NULL == fp_yuv)
        return;

    int width = frame->width;
    int height = frame->height;
    if(width == 0 || height == 0)
    {
        return;
    }
    int i = 0;
    uint8_t* yuv_buf = (uint8_t*)malloc(width * height *1.5);
    for(i = 0; i < height; i++)
    {
        memcpy(yuv_buf + width * i, frame->data[0] + frame->linesize[0]*i, width);
        if(i < height >> 1)
        {
            memcpy(yuv_buf + width * height + width *i / 2, frame->data[1] + frame->linesize[1]*i, width / 2);
            memcpy(yuv_buf + width * height * 5 /4 + width * i / 2 ,frame->data[2] + frame->linesize[2]*i, width / 2);
        }
    }

    //fwrite(yuv_buf, sizeof(uint8_t),width * height * 1.5 ,fp_yuv);
    fwrite(yuv_buf, 1,width * height * 1.5 ,fp_yuv);
    free(yuv_buf);
}
