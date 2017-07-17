#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include "libavutil/samplefmt.h"

#define INBUF_SIZE 40960000
FILE *pcm_fp = NULL;


struct EncoderParam_t{
    AVCodec *codec;
    AVCodecContext *avctx;
    //char *outputName;
    AVFormatContext *format_context;
    AVStream *audio_st;
    FILE *f;
};

struct EncoderParam_t encPar;

static int check_sample_fmt(AVCodec *codec, enum AVSampleFormat sample_fmt)
{
    const enum AVSampleFormat *p = codec->sample_fmts;
    while(*p != AV_SAMPLE_FMT_NONE)
    {
        if(*p == sample_fmt)
            return 1;
        p++;
    }

    return 0;
}

int EncoderInit()
{
    AVIOContext *io_context = NULL;
    AVStream *stream = NULL;

    const char* filename = "itc.aac";
    int error;
    if( (error = avio_open(&io_context, filename, AVIO_FLAG_WRITE)) < 0 )
    {
        printf("Could not open output file %s\n",filename);
        return error;
    }

    encPar.format_context = avformat_alloc_context();
    if(encPar.format_context == NULL)
    {
        printf("Could not allocate output format context\n");
        return -1;
    }

    encPar.format_context->pb = io_context;

    if( !(encPar.format_context->oformat = av_guess_format(NULL, filename, NULL)) )
    {
        printf("Could not found output file format\n");
        return -1;
    }

    av_strlcpy(encPar.format_context->filename, filename, sizeof(encPar.format_context->filename));

    if(!(encPar.codec = avcodec_find_encoder(AV_CODEC_ID_AAC)))
    {
        printf("Could not found an aac encoder\n");
        return -1;
    }

    if( !(stream = avformat_new_stream(encPar.format_context, NULL)) )
    {
        printf("Could not create new stream\n");
        return -1;
    }

    encPar.avctx = avcodec_alloc_context3(encPar.codec);
    if(encPar.avctx == NULL)
    {
        printf("Could not allocate an encoding context\n");
        return -1;
    }

    encPar.avctx->channels = 2;
    encPar.avctx->channel_layout = av_get_default_channel_layout(2);
    encPar.avctx->sample_rate = 44100;
    encPar.avctx->sample_fmt = AV_SAMPLE_FMT_FLTP;
    encPar.avctx->bit_rate = 128000;
    stream->time_base.den = 44100;
    stream->time_base.num = 1;

    if(encPar.format_context->oformat->flags & AVFMT_GLOBALHEADER)
        encPar.avctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    if(avcodec_open2(encPar.avctx, encPar.codec, NULL) < 0)
    {
        printf("Could not open output codec\n");
        return -1;
    }
    
    if(avcodec_parameters_from_context(stream->codecpar, encPar.avctx) < 0)
    {
        printf("Could not initialize stream parameters\n");
        return -1;
    }

#if 1
    if (avformat_write_header(encPar.format_context, NULL) < 0)
    {
        printf("Could not write output file header\n");
        return -1;
    }
#endif

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

    if(encPar.avctx)
        ret = avcodec_encode_audio2(encPar.avctx, &pkt, frame, &got_frame);

    if(got_frame)
    {
        //fwrite(pkt.data,1,pkt.size, encPar.f);
        av_write_frame(encPar.format_context, &pkt);
        av_packet_unref(&pkt);
    }
    return ret;
}

int main(int argc, int argv)
{   
    pcm_fp = fopen("floatSigned32BitLittleEndianStereo_aac.pcm","wb");
    if(NULL == pcm_fp)
    {
        fprintf(stderr,"Open aac.pcm fail\n");
        exit(1);
    }
    
    av_register_all();
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

    codec = avcodec_find_decoder(AV_CODEC_ID_AAC);
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

    avParserContext = av_parser_init(AV_CODEC_ID_AAC);
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

    f = fopen("test.aac","rb");
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

            #if 1
            if(buf_len != 0)
            {
                AVPacket avpkt = {0}; 
                av_init_packet(&avpkt);
                avpkt.data = buf;
                avpkt.size = buf_len;
                int decode_len = avcodec_decode_audio4(c,frame, &got_frame, &avpkt);
                if(decode_len < 0)
                    fprintf(stderr,"Error while decoding frame %d\n",frame_count);
                if(got_frame)
                {
                    Encoder(frame);
                    int data_size = av_get_bytes_per_sample(c->sample_fmt);
                    //printf("decodesample format %s\n",av_get_sample_fmt_name(c->sample_fmt));
                    int i ,ch;
                    if(data_size < 0)
                    {
                        printf("Failed to calculate data size\n");
                        exit(1);
                    }
                    for(i=0; i < frame->nb_samples; i++)
                    {
                        for(ch=  0; ch < c->channels; ch++)
                            fwrite(frame->data[ch] + data_size*i ,1, data_size, pcm_fp);
                    }
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

    //Free memory
    //............
    //............
    fclose(pcm_fp);
    return 0;
}
