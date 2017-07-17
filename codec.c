#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <sys/time.h>

#define INBUF_SIZE 40960000
FILE *yuv_fp = NULL;

struct EncoderParam_t{
    AVCodec *codec;
    AVCodecContext *c;
    char *outputName;
    FILE *f;
};

struct EncoderParam_t encPar;

static int EncoderInit()
{
    encPar.codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if(NULL == encPar.codec)
    {
        printf("Codec not found\n");
        return -1; 
    }

    encPar.c = avcodec_alloc_context3(encPar.codec);
    if(NULL == encPar.c)
    {
        printf("Could not allocate video codec context\n");
        return -1;
    }

    encPar.outputName = "itc.ts";
    encPar.c->bit_rate = 200000000;
    encPar.c->width = 640;
    encPar.c->height = 360;
    encPar.c->time_base = (AVRational){1,25};
    encPar.c->gop_size = 50;
    encPar.c->pix_fmt = AV_PIX_FMT_YUV420P;
    av_opt_set(encPar.c->priv_data, "preset", "slow", 1);

    if( avcodec_open2(encPar.c, encPar.codec, NULL) < 0 )
    {
        printf("Could not open codec\n");
        return -1;
    }

    encPar.f = fopen(encPar.outputName,"wb");
    if(encPar.f == NULL)
    {
        printf("Could not open %s\n",encPar.outputName);
        return -1;
    }
    
    return 0;
}

static int Encoder(AVFrame *frame)
{
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    int ret = 0;
    int got_frame = 0;

    if(encPar.c)
        ret = avcodec_encode_video2(encPar.c, &pkt, frame, &got_frame);

    if(got_frame)
    {
        fwrite(pkt.data,1,pkt.size, encPar.f);
        av_packet_unref(&pkt);
    }
    return ret;
}

int main(int argc, int argv)
{   
    yuv_fp = fopen("test_640x360.yuv","wb");
    if(NULL == yuv_fp)
    {
        fprintf(stderr,"Open test.yuv fail\n");
        exit(1);
    }
    
    avcodec_register_all();
    AVCodec *codec;
    AVCodecContext *c = NULL;
    AVFrame *frame;
    AVCodecParserContext *avParserContext;
    FILE *f = NULL;
    int frame_count = 0;
    //uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    unsigned char * inbuf = (unsigned char*)malloc(INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE);
    int got_frame;
    int read_size;

    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if( NULL == codec )
    {
        fprintf(stderr,"Codec not found\n");
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if(NULL == c)
    {
        fprintf(stderr,"Could not allocate video codec context\n");
        exit(1);
    }
    c->width = 640;
    c->height = 360;
    c->bit_rate = 1000;
    c->time_base.num = 1;
    c->time_base.den = 25;
    c->codec_id = AV_CODEC_ID_H264;
    c->codec_type = AVMEDIA_TYPE_VIDEO;

    avParserContext = av_parser_init(AV_CODEC_ID_H264);
    if( NULL == avParserContext)
    {
        fprintf(stderr,"Could not init avParserContext\n");
        exit(1);
    }

    if(avcodec_open2(c,codec, NULL) < 0)
    {
        fprintf(stderr,"Could not open codec\n");
        exit(1);
    }

    frame = av_frame_alloc();
    if(NULL == frame)
    {
        fprintf(stderr,"Could not allocate video frame\n");
        exit(1);
    }

    f = fopen("test.264","rb");
    if( NULL == f )
    {
        fprintf(stderr,"Could not allocate video frame");
        exit(1);
    }

    EncoderInit();
    for(;;)
    {
        //解码还有问题，因为avpkt不是一帧数据，而是有很多帧的数据，
        //需要增加一个filter，一帧一帧过滤出来
        read_size = fread(inbuf, 1,INBUF_SIZE, f);
        printf("read_size_orig=%d\n",read_size);
        if(read_size == 0)
        {
            break;
        }

        while(read_size)
        {
            unsigned char *buf = 0;
            int buf_len = 0;
            int parse_len = av_parser_parse2(avParserContext, c, &buf, &buf_len,
                        inbuf, read_size,
                        AV_NOPTS_VALUE, AV_NOPTS_VALUE,AV_NOPTS_VALUE);
            inbuf += parse_len;
            read_size -= parse_len;
            //printf("read_size=%d, parse_len=%d, buf_len=%d\n",read_size, parse_len, buf_len);
            #if 1
            if(buf_len != 0)
            {
                AVPacket avpkt = {0}; 
                av_init_packet(&avpkt);
                avpkt.data = buf;
                avpkt.size = buf_len;
                time_t start ,end;
                start = clock();
                int decode_len = avcodec_decode_video2(c,frame, &got_frame, &avpkt);
                end = clock();
                printf("decoded frame used %d\n",end - start);
                if(decode_len < 0)
                    fprintf(stderr,"Error while decoding frame %d\n",frame_count);
                if(got_frame)
                {
                    fprintf(stderr,"decode success\n");
                    int width = frame->width;
                    int height = frame->height;
                    unsigned char* yuv_buf = (unsigned char*)malloc(width * height *1.5);
                    int i = 0;
                    frame->pts+=1;
                    Encoder(frame);

                    //把解码出来的数据存成YUV数据，方便验证解码是否正确
                    for(i = 0; i < height; i++)
                    {
                        memcpy(yuv_buf + width * i, frame->data[0] + frame->linesize[0]*i, width);
                        if(i < height >> 1)
                        {
                            memcpy(yuv_buf + width * height + width *i / 2, frame->data[1] + frame->linesize[1]*i, width / 2);
                            memcpy(yuv_buf + width * height * 5 /4 + width * i / 2 ,frame->data[2] + frame->linesize[2]*i, width / 2);
                        }
                    }
                    fwrite(yuv_buf, sizeof(unsigned char),width * height * 1.5 ,yuv_fp);
                    free(yuv_buf);
                    frame_count++;

                }
                else
                {
                    fprintf(stderr,"decode fail\n");
                }
                av_packet_unref(&avpkt);
            }
            #endif
        }
    }

    return 0;
}
