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

char * host_name = "192.168.10.238";  
static int port_v = 8888;  
static int fd_v = -1;
static int GlobalID = 8888;

#pragma pack(1)
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

void *ReceiveVideoThread(void *args);

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
    framehead->nWidth = 720;
    framehead->nHight = 576;
    framehead->nOthers = GlobalID;
}

int main(int argc, char **argv)
{

    if(init_socket(&fd_v, port_v) < 0)
    {
        printf("Connect video failed\n");
        return 0;
    }

    pthread_create(&handle_video_thread, NULL, ReceiveVideoThread, NULL);
    pthread_join(handle_video_thread,NULL);

    return 0;
}

void *ReceiveVideoThread(void *args)
{
    int i;
    struct FrameHead head_v;
    memset(&head_v, 0, sizeof(struct FrameHead));
    initCheckStartAndCheckEnd(&head_v);
    initFrameHead(&head_v);
    send(fd_v, (char*)&head_v, HEAD_SIZE, 0);
    char* buf = (char*)&head_v;
    for(i=0;i<64;i++)
        printf("buf[%d]=%0x \n",i,buf[i]);
}
