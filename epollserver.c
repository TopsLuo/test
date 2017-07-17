/*-------------------------------------------------------
Date:       2017年 07月 13日 星期四 14:33:07 CST
Author:     TopsLuo
Function:   模拟业务服务器向媒体服务器发送控制命令，
            如create,join,leave,record,stop等。之前的server.c
            都是把这些命令预先写死在程序里，不够灵活，所以
            特意写了该程序，控制命令均由外部脚本(tcp_json)
            发送给该程序
-------------------------------------------------------*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netinet/tcp.h>

#define MYPORT 7777
#define MAX_EVENTS 300000
#define BUF_LEN 1024
int currentClient = 0;

int epollfd;
int mediaServerfd;
struct epoll_event eventList[MAX_EVENTS];

void AcceptConn(int srvfd);
void RecvData(int fd, char *recvBuf, int *recvSize);
void SendData(int fd,char *buf, int size);

int main()
{
    int i, ret, sinSize;
    int recvLen = 0;
    fd_set readfds, writefds;
    int sockListen ,sockSvr, sockMax;
    int timeout;
    struct sockaddr_in sin;
    int nZero = 1;

    sockListen = socket(AF_INET,SOCK_STREAM,0);
    setsockopt(sockListen,SOL_SOCKET,SO_REUSEADDR,&nZero,sizeof(int));
    setsockopt(sockListen,SOL_SOCKET,SO_REUSEPORT,&nZero,sizeof(int));
    setsockopt(sockListen,IPPROTO_TCP,TCP_NODELAY,(char*)&nZero,sizeof(int));
    //int keepAlive = 1;
    //setsockopt(sockListen, SOL_SOCKET, SO_KEEPALIVE,(void *)keepAlive, sizeof(keepAlive));

    bzero(&sin,sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;  
    //地址结构的端口地址，port是主机字节序，通过htons（）进行字节序转换成网络字节序  
    sin.sin_port = htons(MYPORT);
    //将sin的地址和socket文件描述符绑定到一起，绑定是数据接收和发//送的前提  
    if(bind(sockListen,(struct sockaddr *)&sin,sizeof(sin)) == -1)
    {  
        perror("call to bind");  
        exit(1);  
    }  

    if(listen(sockListen, 5) < 0)
    {
        printf("listen error\n");
        return -1;
    }

    epollfd = epoll_create(MAX_EVENTS);
    struct epoll_event event;
    event.events = EPOLLIN|EPOLLET;
    event.data.fd = sockListen;

    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, sockListen, &event) < 0)
    {
        printf("epoll add fail: fd %d\n",sockListen);
        return -1;
    }

    while(1)
    {
        timeout = 40;
        int ret = epoll_wait(epollfd, eventList, MAX_EVENTS, timeout);
        if(ret < 0)
        {
            printf("epoll error\n");
            break;
        }
        else if(ret == 0)
        {
            //printf("timeout ...\n");
            continue;
        }

        int n = 0;
        for(n = 0; n < ret; n++)
        {
            if(eventList[n].events & EPOLLERR ||
               eventList[n].events & EPOLLHUP ||
               !(eventList[n].events & EPOLLIN))
            {
                printf("epoll error\n");
                close(eventList[n].data.fd);
                return -1;
            }

            if(eventList[n].data.fd == sockListen)
            {
                AcceptConn(sockListen);
            }
            else
            {
                char buf[BUF_LEN] = {0};
                int size = 0;
                RecvData(eventList[n].data.fd, buf, &size);
                
                if(size)
                    printf("fd: %d recv: %s,size=%d\n",eventList[n].data.fd, buf, size);

                if(strstr(buf, "register"))
                {
                    //获取媒体服务器的socket，之后接收到的除媒体服务器之后的数据
                    //均需要发送给媒体服务器
                    printf("This is mediaServer socket,Update mediaServer socket\n");
                    mediaServerfd = eventList[n].data.fd;
                }

                if(eventList[n].data.fd != mediaServerfd && size != 0)
                {
                    SendData(mediaServerfd, buf, size);
                }
            }
        }
    }

    close(epollfd);
    close(sockListen);

    return 0;
}

void AcceptConn(int srvfd)
{
    struct sockaddr_in sin;
    socklen_t len = sizeof(struct sockaddr_in);
    bzero(&sin, len);
    
    int confd = accept(srvfd, (struct sockaddr*)&sin, &len);
    
    if(confd < 0)
    {
        printf("bad accept");
        return;
    }
    else{
        printf("Accept Connection: %d\n",confd);
    }

    struct epoll_event event;
    event.data.fd = confd;
    event.events = EPOLLIN|EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, confd, &event);
}

void RecvData(int fd, char *recvBuf, int *recvSize)
{
    int ret;
    int recvLen = 0;

    ret = recv(fd, (char*)recvBuf, BUF_LEN, 0);
    if(ret == 0)
    {
        return;
    }
    else if(ret < 0)
    {
        return;
    }

    *recvSize = ret;
}

void SendData(int fd,char *buf, int size)
{
    int ret = send(fd, buf, size, 0);
    //printf("send: %s,size=%d\n",buf, size);
}
