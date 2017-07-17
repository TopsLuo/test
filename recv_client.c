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

#define HEAD_SIZE 64
#define MAKEFOURCC(ch0, ch1, ch2, ch3)\
        ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1)<<8) | \
        ((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24)) 

#define H264_CODEC_TYPE     MAKEFOURCC('H','2','6','4')
#define AAC_CODEC_TYPE      MAKEFOURCC('A','D','T','S')

char * host_name = "192.168.10.238";  
//char * host_name = "121.41.20.247";  
static int port_v = 8888;  
static int port_a = 8889;  
static int fd_v = -1;
static int fd_a = -1;
static int GlobalID = 0;

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

pthread_t handle_video_thread;
pthread_t handle_audio_thread;

void *ReceiveVideoThread(void *args);
void *ReceiveAudioThread(void *args);
int ParseFrameHead(const unsigned char *buf, struct FrameHead **fhd);
static void ShowFrameHead(const struct FrameHead *fhd);

void initCheckStartAndCheckEnd(struct FrameHead *framehead)
{
    memset(framehead,0,sizeof(struct FrameHead));
    framehead->check_start[0] = framehead->check_start[1] = framehead->check_start[2] = framehead->check_start[3] = '$';
    framehead->check_end[0] = framehead->check_end[1] = framehead->check_end[2] = framehead->check_end[3] = '#';
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

void initFrameHead(struct FrameHead *framehead)
{
    framehead->nFrameLength = 0;
    framehead->nDataCodec = 0;
    framehead->nOthers = GlobalID;
}

int main(int argc, char **argv)
{
    if(argc != 4){
        fprintf(stderr,"exit,\n use it like %s <video_port> <audio_port> <globalID>\n",argv[0]);
        exit(0);
    }
    port_v = atoi(argv[1]);
    port_a = atoi(argv[2]);
    GlobalID = atoi(argv[3]);
    printf("port_v=%d,port_a=%d,GlobalID=%d\n",port_v, port_a, GlobalID);

    if(init_socket(&fd_v, port_v) < 0)
    {
        printf("Connect video failed\n");
        return 0;
    }
    if(init_socket(&fd_a, port_a) < 0)
    {
        printf("Connect audio failed\n");
        return 0;
    }

    pthread_create(&handle_video_thread, NULL, ReceiveVideoThread, NULL);
    pthread_create(&handle_audio_thread, NULL, ReceiveAudioThread, NULL);
    pthread_join(handle_video_thread,NULL);
    pthread_join(handle_audio_thread,NULL);

    return 0;
}

void *ReceiveVideoThread(void *args)
{
    char filename[64];
    sprintf(filename, "../dataTestDir/receiveVideo_%d.264", GlobalID);
    FILE *fp = fopen(filename, "wb");
    struct FrameHead head_v;
    initCheckStartAndCheckEnd(&head_v);
    initFrameHead(&head_v);
    send(fd_v, (unsigned char*)&head_v, HEAD_SIZE, 0);
    struct FrameHead *head = NULL;
    char buf[HEAD_SIZE];
    while(1)
    {
        if(head == NULL)
        {
            int len = recv(fd_v, buf, HEAD_SIZE, 0);
            if(ParseFrameHead(buf, &head))
            {
                ShowFrameHead(head);
            }
        }
        else{
            unsigned char *data = (unsigned char*)malloc(head->nFrameLength + 1);
            memset(data, 0, head->nFrameLength+1);
            int skip_len = 0;
            int recv_len = 0;
            while(1)
            {
                int len = recv(fd_v, data + skip_len, head->nFrameLength - skip_len, 0);
                if(len == 0)
                {
                    printf("Connect is close\n");
                    exit(0);
                }
                else if(len < 0)
                {
                    continue;
                }
                recv_len += len;
                if(recv_len == head->nFrameLength)
                {
                    break;
                }
                else{
                    skip_len += len;
                }
            }
            fwrite(data, 1, head->nFrameLength, fp);
            free(data);
            data = NULL;
            head = NULL;
        }
    }
    fclose(fp);
}

void *ReceiveAudioThread(void *args)
{
    char filename[64];
    sprintf(filename, "../dataTestDir/receiveAudio_%d.aac", GlobalID);
    FILE *fp = fopen(filename, "wb");
    struct FrameHead head_a;
    initCheckStartAndCheckEnd(&head_a);
    initFrameHead(&head_a);
    send(fd_a, (unsigned char*)&head_a, HEAD_SIZE, 0);
    struct FrameHead *head = NULL;
    char buf[HEAD_SIZE];
    while(1)
    {
        if(head == NULL)
        {
            int len = recv(fd_a, buf, HEAD_SIZE, 0);
            if(ParseFrameHead(buf, &head))
            {
                ShowFrameHead(head);
            }
        }
        else{
            unsigned char *data = (unsigned char*)malloc(head->nFrameLength + 1);
            memset(data, 0, head->nFrameLength+1);
            int skip_len = 0;
            int recv_len = 0;
            while(1)
            {
                int len = recv(fd_a, data + skip_len, head->nFrameLength - skip_len, 0);
                if(len == 0)
                {
                    printf("Connect is close\n");
                    exit(0);
                }
                else if(len < 0)
                {
                    continue;
                }
                recv_len += len;
                if(recv_len == head->nFrameLength)
                {
                    break;
                }
                else{
                    skip_len += len;
                }
            }
            fwrite(data, 1, head->nFrameLength, fp);
            free(data);
            data = NULL;
            head = NULL;
        }
    }
    fclose(fp);
}

int ParseFrameHead(const unsigned char *buf, struct FrameHead **fhd)
{
    int i;
    if( buf[0] == '$' && buf[1] == '$' && buf[2] == '$' && buf[3] == '$')
    {
        *fhd = (struct FrameHead*)buf;
        return 1;
    }
    printf("This is not frame head packet\n");
    return 0;
}

static void ShowFrameHead(const struct FrameHead *fhd)
{
    printf("ID:%d\n", fhd->ID );
    printf("nChannel:%d\n", fhd->nChannel );
    printf("nPTimeTick:%d\n", fhd->nPTimeTick );
    printf("nDTimeTick:%d\n", fhd->nDTimeTick );
    printf("nFrameLength:%d\n", fhd->nFrameLength );
    printf("nDataCodec:%d\n", fhd->nDataCodec );
    printf("s_blue:%d\n", fhd->s_blue );
    printf("nPacketNumber:%d\n", fhd->nPacketNumber );
    printf("nOthers:%d\n", fhd->nOthers );//GlobalID
    if(fhd->nDataCodec == AAC_CODEC_TYPE)
    {
        printf("nSamplerate:%d\n", fhd->nSamplerate );
        printf("nAudioChannel:%d\n", fhd->nAudioChannel );
        printf("nSamplebit:%d\n", fhd->nSamplebit );
        printf("nBandwidth:%d\n", fhd->nBandwidth );
        printf("nReserve:%d\n", fhd->nReserve );
    }
    else
    {
        printf("nFrameRate:%d\n", fhd->nFrameRate );
        printf("nWidth:%d\n", fhd->nWidth );
        printf("nHight:%d\n", fhd->nHight );
        printf("nColors:%d\n", fhd->nColors );
        printf("nIframe:%d\n", fhd->nIframe );
    }
}
