#include <sys/socket.h>  
#include <sys/types.h>  
#include <unistd.h>   
#include <arpa/inet.h>  
#include <netinet/in.h>  
#include <errno.h>  
#include <strings.h>  
#include <ctype.h>   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
               
char host_name[20];  
int port = 7777;  
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

void RecvAndSendThread(void *argv)
{
    int* sock_fd = (int*)argv; 
    char buf[65535];  
    int idx = 0;
    int ret = 0;
    struct FrameHead *fhd;
    //while(1)
    {
        int len = recv(*sock_fd, buf,65535,0);
        if(len == -1)
        {
            perror("call to recv");
            exit(1);
        }
        fhd = (struct FrameHead*)&buf;
        printf("fhd->nOthers=%d\n",fhd->nOthers);
        printf("fhd->nWidth=%d\n",fhd->nWidth);
    }
}

int main()  
{     
    struct sockaddr_in sin,pin;
    int sock_descriptor,temp_sock,address_size;  
    int i , len , on=1;  
    int nZero = 1;
    char buf[16384];  
      
    //通过socket(int domain,int type,int protocol)的函数原型创建AF_INIT协议族的流
    //类型的socket，当然type除了流socket的之外还有SOCKET_RAW,
    //据说这种类型的socket甚至可以伪装数据包
    sock_descriptor = socket(AF_INET,SOCK_STREAM,0);
    setsockopt(sock_descriptor,SOL_SOCKET,SO_REUSEADDR,&nZero,sizeof(int));
    setsockopt(sock_descriptor,SOL_SOCKET,SO_REUSEPORT,&nZero,sizeof(int));
    //int keepAlive = 1;
    //setsockopt(sock_descriptor, SOL_SOCKET, SO_KEEPALIVE,(void *)keepAlive, sizeof(keepAlive));

    bzero(&sin,sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;  
    //地址结构的端口地址，port是主机字节序，通过htons（）进行字节序转换成网络字节序  
    sin.sin_port = htons(port);
    //将sin的地址和socket文件描述符绑定到一起，绑定是数据接收和发//送的前提  
    if(bind(sock_descriptor,(struct sockaddr *)&sin,sizeof(sin)) == -1)
    {  
        perror("call to bind");  
        exit(1);  
    }  
    //监听用来初始化服务器可连接队列，因为一次只能处理一个连接请求
    //当收到多个请求，将会存储在队列中,先到先得  
    if(listen(sock_descriptor,100) == -1)
    {  
        perror("call to listem");  
        exit(1);  
    }  
    printf("Accpting connections...\n");  
  
    while(1)  
    {  
        address_size = sizeof(pin);  

        temp_sock = accept(sock_descriptor,(struct sockaddr *)&pin,&address_size);
        printf("temp_sock=%d\n",temp_sock);
        if(temp_sock == -1)  
        {  
            perror("call to accept");  
            exit(1);  
        }  
        usleep(1000 * 1000);
        pthread_t thread_id;
        pthread_create(&thread_id,NULL,(void*)RecvAndSendThread,&temp_sock);
        //close(temp_sock);  
    }  
}  

