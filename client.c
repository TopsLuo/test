#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <sys/socket.h>  
#include <sys/types.h>  
#include <unistd.h>  
#include <arpa/inet.h>  
#include <netinet/in.h>  
#include <errno.h>  
#include <strings.h>  
#include <ctype.h>  
#include <sys/time.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libavutil/time.h>
  
#define MAKEFOURCC(ch0, ch1, ch2, ch3)\
        ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1)<<8) | \
        ((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24)) 

#define H264_CODEC_TYPE     MAKEFOURCC('H','2','6','4')
#define AAC_CODEC_TYPE      MAKEFOURCC('A','D','T','S')
#define RQHD_CODEC_TYPE     MAKEFOURCC('R','Q','H','D')
#define JPEG_CODEC_TYPE     MAKEFOURCC('J','P','E','G')
#define HEAD_ID             MAKEFOURCC('4','D','S','P')
#define HEAD_SIZE 64
#define AAC_FILE 0
  
//char * host_name = "121.41.20.247";  
char * host_name = "192.168.10.238";  
static int port_v = 8888;  
static int port_a = 8889;  
static int fd_v = -1;
static int fd_a = -1;
static unsigned int packet_v = 0;
static unsigned int packet_a = 0;
static unsigned total_send = 0;
static int GlobalID = 0;
static uint64_t counter = 0;
static int orig_width = 0;
static int orig_height = 0;
static int loop = 0;
AVFormatContext *ifmt_ctx;
AVFormatContext *format_context = NULL;
AVCodecContext *avctx_v;
AVCodecContext *avctx_a;

#if DEBUG 
FILE *fp_v;
FILE *fp_a;
#endif


#define LEN 128
unsigned idx = 0;

struct FrameHead{
    unsigned char check_start[4];
    unsigned int ID;
    unsigned int nChannel;
    unsigned int nPTimeTick;
    unsigned int nDTimeTick;
    unsigned int nFrameLength;
    unsigned int nDataCodec;
    unsigned int s_blue;
    union{
        unsigned int nFrameRate;
        unsigned int nSamplerate;
    };
    union{
        unsigned int nWidth;
        unsigned int nAudioChannel;
    };
    union{
        unsigned int nHight;
        unsigned int nSamplebit;
    };
    union{
        unsigned int nColors;
        unsigned int nBandwidth;
    };
    union{
        unsigned int nIframe;
        unsigned int nReserve;
    };
    unsigned int nPacketNumber;
    unsigned int nOthers;
    unsigned char check_end[4];
};

struct FrameHead head_v;
struct FrameHead head_a;

void initCheckStartAndCheckEnd(struct FrameHead *framehead)
{
    memset(framehead,0,sizeof(struct FrameHead));
    framehead->check_start[0] = framehead->check_start[1] = framehead->check_start[2] = framehead->check_start[3] = '$';
    framehead->check_end[0] = framehead->check_end[1] = framehead->check_end[2] = framehead->check_end[3] = '#';
}

void initFrameHead_v(struct FrameHead *framehead,AVPacket *pkt, AVFrame *frame)
{
    initCheckStartAndCheckEnd(framehead);
    framehead->ID = HEAD_ID;
    framehead->nChannel = 2;
    framehead->nPTimeTick = pkt->pts;
    framehead->nDTimeTick = pkt->dts;
    framehead->nFrameLength = pkt->size;
    framehead->nDataCodec = H264_CODEC_TYPE;
    framehead->s_blue = 0;
    framehead->nFrameRate = 25;
    framehead->nWidth= frame->width;
    framehead->nHight = frame->height;
    framehead->nColors = 16;
    framehead->nIframe = 1;
    framehead->nPacketNumber = packet_v++;
    framehead->nOthers = GlobalID;//GlobalID
}

void initFrameHead_a(struct FrameHead *framehead,AVPacket *pkt, AVFrame *frame)
{
    initCheckStartAndCheckEnd(framehead);
    framehead->ID = HEAD_ID;
    framehead->nChannel = 2;
    framehead->nPTimeTick = pkt->pts;
    framehead->nDTimeTick = pkt->dts;
    framehead->nFrameLength = pkt->size + 7;
    framehead->nDataCodec = AAC_CODEC_TYPE;
    framehead->s_blue = 0;
    framehead->nSamplerate = 44100;
    framehead->nAudioChannel= 2;
    framehead->nSamplebit = 32;
    framehead->nBandwidth = 12800;
    framehead->nReserve = 0;
    framehead->nPacketNumber = packet_a++;
    framehead->nOthers = GlobalID;//GlobalID
}
int adts_sample_rates[] = {96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000,7350,0,0,0};

static int FindAdtsSRIndex(int sr)
{
    int i;
    for(i = 0;i < 16; i++)
    {
        if(sr == adts_sample_rates[i])
            return i;
    }
    return 16 - 1;
}

unsigned char *MakeAdtsHeader(int *dataSize, int samplerate, int channels ,int iFrameLen)
{
    unsigned char *data;
    int profile = 1;
    int sr_index = FindAdtsSRIndex(samplerate);
    int skip = 7;
    int framesize = skip + iFrameLen;
    *dataSize = 7;

    data = (unsigned char*)malloc( (*dataSize)*sizeof(unsigned char) );
    memset(data, 0, (*dataSize) * sizeof(unsigned char));
    data[0] += 0xFF;
    
    data[1] += 0xF0;
    data[1] += 1;

    data[2] += ((profile << 6) & 0xC0);
    data[2] += ((sr_index << 2) & 0x30);
    data[2] += ((channels >> 2) & 0x1);

    data[3] += ((channels << 6) & 0xC0);
    data[3] += ((framesize >> 11) & 0x3);

    data[4] += ((framesize >> 3) & 0xFF);

    data[5] += ((framesize << 5) & 0xE0);
    data[5] += ((0X7FF >> 6) & 0x1F);

    data[6] += ((0X7FF << 2) & 0x3F);
    return data;
}

static int init_socket(int *fd, int port)
{
    int nZero = 1;
    struct sockaddr_in pin;  
    bzero(&pin,sizeof(pin));  
    pin.sin_family = AF_INET;  
    inet_pton(AF_INET,host_name,&pin.sin_addr);  
    pin.sin_port = htons(port);  
    if((*fd=  socket(AF_INET,SOCK_STREAM,0)) == -1)
    {  
        perror("error opening socket \n");  
        return -1;
    }  

    setsockopt(*fd, SOL_SOCKET,SO_REUSEADDR,&nZero,sizeof(int));
    setsockopt(*fd, SOL_SOCKET,SO_REUSEPORT,&nZero,sizeof(int));

    if(connect(*fd,(struct sockaddr * )&pin,sizeof(pin)) == -1)
    {  
        perror("error connecting to socket \n");  
        return -1;
    }  
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
        av_log(NULL,AV_LOG_ERROR,"Cannot open input filename\n");
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
        if(codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            orig_width = codec_ctx->width;
            orig_height = codec_ctx->height;
            printf("width=%d, height=%d\n",orig_width, orig_height);
        }
    }
    
    av_dump_format(ifmt_ctx, 0, filename ,0);
}

static int VideoEncoderInit()
{
    AVCodec *codec;
    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if(codec == NULL)
    {
        av_log(NULL,AV_LOG_ERROR,"codec not found\n");
        return -1;
    }

    avctx_v = avcodec_alloc_context3(codec);
    if(avctx_v == NULL)
    {
        av_log(NULL,AV_LOG_ERROR,"Could not allocate video codec context\n");
        return -1;
    }

    avctx_v->bit_rate = 200000;
    avctx_v->width = orig_width;
    avctx_v->height = orig_height;
    avctx_v->time_base= (AVRational){1,25};
    avctx_v->gop_size = 50;
    avctx_v->pix_fmt = AV_PIX_FMT_YUV420P;
    av_opt_set(avctx_v, "preset","ultrafast",1);

    if(avcodec_open2(avctx_v, codec, NULL) < 0)
    {
        av_log(NULL,AV_LOG_ERROR,"Could not open codec\n");
        return -1;
    }
    return 0;
}

static int AudioEncoderInit()
{
#if !AAC_FILE
    AVCodec *codec;
    codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if(codec == NULL)
    {
        av_log(NULL,AV_LOG_ERROR,"codec not found\n");
        return -1;
    }

    avctx_a = avcodec_alloc_context3(codec);
    if(avctx_a == NULL)
    {
        av_log(NULL,AV_LOG_ERROR,"Could not allocate video codec context\n");
        return -1;
    }

    avctx_a->bit_rate = 128000;
    avctx_a->channels = 2;
    avctx_a->channel_layout = av_get_default_channel_layout(2);
    avctx_a->sample_rate = 44100;
    avctx_a->sample_fmt = AV_SAMPLE_FMT_FLTP;
    avctx_a->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    if(avcodec_open2(avctx_a, codec, NULL) < 0)
    {
        av_log(NULL,AV_LOG_ERROR,"Could not open codec\n");
        return -1;
    }
    return 0;
//#endif

#else
    AVIOContext *io_context = NULL;
    AVStream *stream = NULL;
    //AVFormatContext *format_context = NULL;
    AVCodec *codec = NULL;

    const char* filename = "itc.aac";
    int error;
    if( (error = avio_open(&io_context, filename, AVIO_FLAG_WRITE)) < 0 )
    {
        printf("Could not open output file %s\n",filename);
        return error;
    }

    format_context = avformat_alloc_context();
    if(format_context == NULL)
    {
        printf("Could not allocate output format context\n");
        return -1;
    }

    format_context->pb = io_context;

    if( !(format_context->oformat = av_guess_format(NULL, filename, NULL)) )
    {
        printf("Could not found output file format\n");
        return -1;
    }

    av_strlcpy(format_context->filename, filename, sizeof(format_context->filename));

    if(!(codec = avcodec_find_encoder(AV_CODEC_ID_AAC)))
    {
        printf("Could not found an aac encoder\n");
        return -1;
    }

    if( !(stream = avformat_new_stream(format_context, NULL)) )
    {
        printf("Could not create new stream\n");
        return -1;
    }

    avctx_a = avcodec_alloc_context3(codec);
    if(avctx_a == NULL)
    {
        printf("Could not allocate an encoding context\n");
        return -1;
    }

    avctx_a->channels = 2;
    avctx_a->channel_layout = av_get_default_channel_layout(2);
    avctx_a->sample_rate = 44100;
    avctx_a->sample_fmt = AV_SAMPLE_FMT_FLTP;
    avctx_a->bit_rate = 128000;
    stream->time_base.den = 44100;
    stream->time_base.num = 1;

    if(format_context->oformat->flags & AVFMT_GLOBALHEADER)
        avctx_a->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    if(avcodec_open2(avctx_a, codec, NULL) < 0)
    {
        printf("Could not open output codec\n");
        return -1;
    }
    
    if(avcodec_parameters_from_context(stream->codecpar, avctx_a) < 0)
    {
        printf("Could not initialize stream parameters\n");
        return -1;
    }

    if (avformat_write_header(format_context, NULL) < 0)
    {
        printf("Could not write output file header\n");
        return -1;
    }
    return 0;
#endif
}

static int VideoEncoder(AVFrame *frame)
{
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    int ret = 0;
    int got_frame = 0;
    if(avctx_v)
    {
        time_t start ,end;
        start = clock();
        ret = avcodec_encode_video2(avctx_v, &pkt, frame, &got_frame);
        end = clock();
        if(ret < 0)
        {
            av_log(NULL,AV_LOG_ERROR,"Encode failed\n");
            return ret;
        }

        if(got_frame)
        {
            //printf("encoded used %d\n",end - start);
            //av_log(NULL,AV_LOG_ERROR,"Encode successful\n");
            //fwrite(pkt.data, 1, pkt.size, fp_v);
            /*if(send(fd_v, pkt.data, pkt.size, 0) == -1)
            {
                av_log(NULL,AV_LOG_ERROR,"Error in send\n");
            }*/


            int len = pkt.size;
            unsigned char *tmp = pkt.data;

            initFrameHead_v(&head_v, &pkt, frame);
            send(fd_v, (char*)&head_v, HEAD_SIZE, 0);
            total_send += 64;

#if DEBUG
            fwrite(pkt.data, 1, pkt.size, fp_v);
#endif
            while(len > 0)
            {
                int send_len = send(fd_v, tmp, len, 0);
                //int send_len = send(fd_v, tmp, 300, 0);
                if(send_len == -1)
                {
                    av_log(NULL,AV_LOG_ERROR,"Error in send\n");
                }
                len -= send_len;
                tmp += send_len;
                total_send += send_len;
                //printf("video -%d,total_send = %d,current_send=%d\n",len,total_send,send_len);
            }
            //printf("globalid=%d,counter=%d,frame_size=%d\n",GlobalID,counter++,pkt.size);

            av_packet_unref(&pkt);
        }

    }

}

static int AudioEncoder(AVFrame *frame)
{
    int ret;
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    int got_frame = 0;

    if(avctx_a)
        ret = avcodec_encode_audio2(avctx_a, &pkt, frame, &got_frame);

    if(got_frame)
    {
        //printf("encode audio frame successful\n");
        int adts_len;
        int samplerate = 44100;
        int channels = 2;

        unsigned char *data = MakeAdtsHeader(&adts_len, samplerate, channels ,pkt.size);
#if AAC_FILE
        av_write_frame(format_context, &pkt);
#else
        int len = pkt.size;
        unsigned char *tmp = pkt.data;

        initFrameHead_a(&head_a, &pkt, frame);
        send(fd_a, (char*)&head_a, HEAD_SIZE, 0);
        send(fd_a, data, 7, 0);
        free(data);
        total_send += 64;

        while(len > 0)
        {
            int send_len = send(fd_a, tmp, len, 0);
            if(send_len == -1)
            {
                av_log(NULL,AV_LOG_ERROR,"Error in send\n");
            }
            len -= send_len;
            tmp += send_len;
            total_send += send_len;
            //printf("audio -= %d,total_send = %d,current_send=%d\n",len,total_send,send_len);
        }

#endif
        av_packet_unref(&pkt);
    }
    return ret;
}

int main(int argc , char * argv[])  
{  
    int reg;
    AVPacket packet = {.data = NULL, .size = 0};
    AVFrame *frame = NULL;
    enum AVMediaType type;
    unsigned int stream_index;
    unsigned int i;
    int ret;
    int got_frame;
    int (*dec_func)(AVCodecContext *, AVFrame *, int *, const AVPacket *);

    if(argc != 5){
        av_log(NULL,AV_LOG_ERROR,"exit,\n use it like %s <inputfile> <video_port> <audio_port> <globalID>\n",argv[0]);
        exit(0);
    }
    GlobalID = atoi(argv[4]);

    av_register_all();

    if(ret = open_input_file(argv[1]) < 0)
    {
        av_log(NULL,AV_LOG_ERROR,"open file failed\n"); 
        exit(1);
    }
    port_v = atoi(argv[2]);
    port_a = atoi(argv[3]);
    printf("video port = %d,audio port = %d\n",port_v ,port_a);

    init_socket(&fd_v, port_v);
    init_socket(&fd_a, port_a);
    //sleep(10);
    //exit(0);

    if( fd_v == -1 || fd_a == -1 )
    {
        printf("init socket failed\n");
        //exit(0);
    }
    printf("fd_v = %d,fd_a=%d\n",fd_v, fd_a);

    if(VideoEncoderInit() < 0 )
    {
        printf("VideoEncoderInit failed\n");
        exit(0);
    }

    if(AudioEncoderInit() < 0 )
    {
        printf("AudioEncoderInit failed\n");
        exit(0);
    }


#if DEBUG
    fp_v = fopen("encode.264","wb");
    if(fp_v ==NULL)
    {
        av_log(NULL,AV_LOG_ERROR,"open file failed\n");
        exit(0);
    }
    fp_a = fopen("encode.aac","wb");
    if(fp_a ==NULL)
    {
        av_log(NULL,AV_LOG_ERROR,"open file failed\n");
        exit(0);
    }
#endif

    frame = av_frame_alloc();
    if(frame == NULL)
    {
        av_log(NULL,AV_LOG_ERROR,"alloc frame failed\n");
        exit(0);
    }

    int64_t start = av_gettime_relative();
    while(1)
    {
        //usleep(10*1000);
        ret = av_read_frame(ifmt_ctx, &packet);
        if(ret < 0)
        {
            if(0)//loop
            {
                av_log(NULL,AV_LOG_ERROR,"Read end,Loop again\n");
                av_seek_frame(ifmt_ctx, -1, ifmt_ctx->start_time, 0);
                continue;
            }
            else
            {
                av_log(NULL,AV_LOG_ERROR,"Read end\n");
                break;
            }
        }

        stream_index = packet.stream_index;

        //转换时间，根据时间来判断是否需要等待，来控制实时传输
        int64_t now = av_gettime_relative() - start;
        AVRational tb = ifmt_ctx->streams[stream_index]->time_base;
        int64_t pts = av_rescale_q(packet.pts, tb, AV_TIME_BASE_Q);
        if(packet.pts == AV_NOPTS_VALUE)
        {
            //send this frame
            printf("%"PRId64"\n",packet.pts);
        }
        else if((pts > now) && (now > 0))
        {
            printf("now:%lld, pts:%lld\n",now, pts);
            usleep(pts - now);
        }

        type = ifmt_ctx->streams[stream_index]->codec->codec_type;
        dec_func = (type == AVMEDIA_TYPE_VIDEO) ? avcodec_decode_video2 : avcodec_decode_audio4;
        time_t start ,end;
        if(type == AVMEDIA_TYPE_VIDEO)
        {
            start = clock();
        }
        ret = dec_func(ifmt_ctx->streams[stream_index]->codec, frame, &got_frame, &packet);
        if(type == AVMEDIA_TYPE_VIDEO)
        {
            end = clock();
            //printf("decode used %d\n",end - start);
        }
        if(ret < 0)
        {
            av_log(NULL,AV_LOG_ERROR,"Decoding failed\n");
            break;
        }
        if(got_frame)
        {
            //av_log(NULL,AV_LOG_ERROR,"Decoding successful\n");
            if(type == AVMEDIA_TYPE_VIDEO)
            {
                frame->pict_type = AV_PICTURE_TYPE_NONE;
                VideoEncoder(frame);
            }
            else if(type == AVMEDIA_TYPE_AUDIO)
            {
                AudioEncoder(frame);
            }

            av_packet_unref(&packet);
        }
        
    }
    av_frame_free(&frame);
    for(i = 0; i < ifmt_ctx->nb_streams; i++)
    {
        avcodec_close(ifmt_ctx->streams[i]->codec);
    }
    avformat_close_input(&ifmt_ctx);
    avcodec_close(avctx_v);
    avcodec_close(avctx_a);
#if DEBUG
    fclose(fp_v);
    fclose(fp_a);
#endif
    return 1;  
}  
