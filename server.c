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
#include <netinet/tcp.h>
               
char host_name[20];  
int port = 7777;  

char *create_json ="{\"room_id\" : 123,\"message_type\" : \"create\", \"to\" : \"media_server\",\"creator\" : \"admin\",\"room_type\" : 1,\"room_name\" : \"test\", \"duration\" : 30, \"passwd\" : \"TopsLuo\", \"video\" : {\"video_codec\" : \"h264\",\"width\" : 1920, \"height\" : 1080, \"bitrate\" : 2000000, \"framerate\" : 25, \"kint\" : 50, \"bframe\" : 1},\"audio\" : {\"audio_codec\" : \"libfaac\", \"samplerate\" : 44100,\"channel\" : 2,\"bitrate\" : 128000}}" ;

char *create_01_json ="{\"room_id\" : 124,\"message_type\" : \"create\", \"to\" : \"media_server\",\"creator\" : \"admin\",\"room_type\" : 1,\"room_name\" : \"test124\", \"duration\" : 30, \"passwd\" : \"TopsLuo\", \"video\" : {\"video_codec\" : \"h264\",\"width\" : 640, \"height\" : 360, \"bitrate\" : 2000000, \"framerate\" : 25, \"kint\" : 25, \"bframe\" : 2},\"audio\" : {\"audio_codec\" : \"libfaac\", \"samplerate\" : 44100,\"channel\" : 2,\"bitrate\" : 128}}" ;

char *join_00_json = "{\"room_id\" : 123, \"message_type\" : \"join\",\"to\" : \"medis_server\",\"stream_type\" : 3,\"device_type\" : \"minicc_2\", \"device_sn\" : \"device_00\",\"global_id\" : 1000,\"coordinater\" : \"TopsLuo\",\"passwd\" : \"killer\",\"video\" : 1,\"audio\" : 1}";
char *join_01_json = "{\"room_id\" : 123, \"message_type\" : \"join\",\"to\" : \"medis_server\",\"stream_type\" : 3,\"device_type\" : \"minicc_2\", \"device_sn\" : \"device_01\",\"global_id\" : 1001,\"coordinater\" : \"TopsLuo\",\"passwd\" : \"killer\",\"video\" : 0,\"audio\" : 1}";
char *join_02_json = "{\"room_id\" : 123, \"message_type\" : \"join\",\"to\" : \"medis_server\",\"stream_type\" : 3,\"device_type\" : \"minicc_2\", \"device_sn\" : \"device_02\",\"global_id\" : 1002,\"coordinater\" : \"TopsLuo\",\"passwd\" : \"killer\",\"video\" : 1,\"audio\" : 1}";
char *join_03_json = "{\"room_id\" : 123, \"message_type\" : \"join\",\"to\" : \"medis_server\",\"stream_type\" : 3,\"device_type\" : \"minicc_2\", \"device_sn\" : \"device_03\",\"global_id\" : 1003,\"coordinater\" : \"TopsLuo\",\"passwd\" : \"killer\",\"video\" : 1,\"audio\" : 1}";
char *join_04_json = "{\"room_id\" : 123, \"message_type\" : \"join\",\"to\" : \"medis_server\",\"stream_type\" : 3,\"device_type\" : \"minicc_2\", \"device_sn\" : \"device_04\",\"global_id\" : 1004,\"coordinater\" : \"TopsLuo\",\"passwd\" : \"killer\",\"video\" : 1,\"audio\" : 1}";
char *join_05_json = "{\"room_id\" : 123, \"message_type\" : \"join\",\"to\" : \"medis_server\",\"stream_type\" : 3,\"device_type\" : \"minicc_2\", \"device_sn\" : \"device_05\",\"global_id\" : 1005,\"coordinater\" : \"TopsLuo\",\"passwd\" : \"killer\",\"video\" : 1,\"audio\" : 1}";
char *join_06_json = "{\"room_id\" : 123, \"message_type\" : \"join\",\"to\" : \"medis_server\",\"stream_type\" : 3,\"device_type\" : \"minicc_2\", \"device_sn\" : \"device_06\",\"global_id\" : 1006,\"coordinater\" : \"TopsLuo\",\"passwd\" : \"killer\",\"video\" : 1,\"audio\" : 1}";
char *join_07_json = "{\"room_id\" : 123, \"message_type\" : \"join\",\"to\" : \"medis_server\",\"stream_type\" : 3,\"device_type\" : \"minicc_2\", \"device_sn\" : \"device_07\",\"global_id\" : 1007,\"coordinater\" : \"TopsLuo\",\"passwd\" : \"killer\",\"video\" : 1,\"audio\" : 1}";
char *join_08_json = "{\"room_id\" : 123, \"message_type\" : \"join\",\"to\" : \"medis_server\",\"stream_type\" : 3,\"device_type\" : \"minicc_2\", \"device_sn\" : \"device_08\",\"global_id\" : 1008,\"coordinater\" : \"TopsLuo\",\"passwd\" : \"killer\",\"video\" : 1,\"audio\" : 1}";
char *join_09_json = "{\"room_id\" : 123, \"message_type\" : \"join\",\"to\" : \"medis_server\",\"stream_type\" : 3,\"device_type\" : \"minicc_2\", \"device_sn\" : \"device_09\",\"global_id\" : 1009,\"coordinater\" : \"TopsLuo\",\"passwd\" : \"killer\",\"video\" : 1,\"audio\" : 1}";
char *join_10_json = "{\"room_id\" : 123, \"message_type\" : \"join\",\"to\" : \"medis_server\",\"stream_type\" : 3,\"device_type\" : \"minicc_2\", \"device_sn\" : \"device_10\",\"global_id\" : 1010,\"coordinater\" : \"TopsLuo\",\"passwd\" : \"killer\",\"video\" : 1,\"audio\" : 1}";
char *join_11_json = "{\"room_id\" : 123, \"message_type\" : \"join\",\"to\" : \"medis_server\",\"stream_type\" : 3,\"device_type\" : \"minicc_2\", \"device_sn\" : \"device_11\",\"global_id\" : 1011,\"coordinater\" : \"TopsLuo\",\"passwd\" : \"killer\",\"video\" : 1,\"audio\" : 1}";
char *join_12_json = "{\"room_id\" : 123, \"message_type\" : \"join\",\"to\" : \"medis_server\",\"stream_type\" : 3,\"device_type\" : \"minicc_2\", \"device_sn\" : \"device_12\",\"global_id\" : 1012,\"coordinater\" : \"TopsLuo\",\"passwd\" : \"killer\",\"video\" : 1,\"audio\" : 1}";
char *join_13_json = "{\"room_id\" : 123, \"message_type\" : \"join\",\"to\" : \"medis_server\",\"stream_type\" : 3,\"device_type\" : \"minicc_2\", \"device_sn\" : \"device_13\",\"global_id\" : 1013,\"coordinater\" : \"TopsLuo\",\"passwd\" : \"killer\",\"video\" : 1,\"audio\" : 1}";
char *join_14_json = "{\"room_id\" : 123, \"message_type\" : \"join\",\"to\" : \"medis_server\",\"stream_type\" : 3,\"device_type\" : \"minicc_2\", \"device_sn\" : \"device_14\",\"global_id\" : 1014,\"coordinater\" : \"TopsLuo\",\"passwd\" : \"killer\",\"video\" : 1,\"audio\" : 1}";
char *join_15_json = "{\"room_id\" : 123, \"message_type\" : \"join\",\"to\" : \"medis_server\",\"stream_type\" : 3,\"device_type\" : \"minicc_2\", \"device_sn\" : \"device_15\",\"global_id\" : 1015,\"coordinater\" : \"TopsLuo\",\"passwd\" : \"killer\",\"video\" : 1,\"audio\" : 1}";

char *leave_json = "{\"room_id\" : 123, \"message_type\" : \"leave\",\"to\" : \"media_server\", \"device_type\" : \"minicc_2\", \"device_sn\" : \"device_00\"}";

char *stop_json = "{\"room_id\" : 123, \"message_type\" : \"stop\",\"to\" : \"media_server\", \"requestor\" : \"admin\", \"passwd\" : \"killer\"}";

char *videomix_json = "{\"room_id\" : 123, \"message_type\" : \"video_mix\",\"to\" : \"media_server\", \"requestor\" : \"admin\", \"source_number\" : 5, \"video_mixer\" : [\"device_00\",\"device_01\",\"device_02\",\"device_03\",\"device_04\"], \"layout\" : \"layout_06_1P5\"}";

char *audiomix_json = "{\"room_id\" : 123, \"message_type\" : \"audio_mix\",\"to\" : \"media_server\", \"requestor\" : \"admin\", \"source_number\" : 3, \"audio_mixer\" : [\"device_00\",\"device_01\", \"device_02\"]}";
  
char *record_json ="{\"room_id\" : 123,\"message_type\" : \"start_record\", \"to\" : \"media_server\",\"creator\" : \"admin\",\"file_name\" : \"test.mp4\",\"nots\" : \"test\",\"file_type\" : \"mp4\", \"duration\" : 30, \"max_size\" : 300, \"video\" : {\"video_codec\" : \"h264\",\"width\" : 720, \"height\" : 576, \"bitrate\" : 1200000, \"framerate\" : 30, \"kint\" : 50, \"bframe\" : 1},\"audio\" : {\"audio_codec\" : \"libfaac\", \"samplerate\" : 44100,\"channel\" : 2,\"bitrate\" : 64000}}" ;

char *stop_record_json = "{\"room_id\" : 123, \"message_type\" : \"stop_record\",\"to\" : \"media_server\", \"requestor\" : \"admin\", \"passwd\" : \"killer\"}";
char *pause_record_json = "{\"room_id\" : 123, \"message_type\" : \"pause_record\",\"to\" : \"media_server\", \"requestor\" : \"admin\", \"passwd\" : \"killer\"}";
char *resume_record_json = "{\"room_id\" : 123, \"message_type\" : \"resume_record\",\"to\" : \"media_server\", \"requestor\" : \"admin\", \"passwd\" : \"killer\"}";
void RecvAndSendThread(void *argv)
{
    int* sock_fd = (int*)argv; 
    char buf[65535];  
    int idx = 0;
    int ret = 0;
    //FILE *fp = fopen("server.264","wb");
    while(1)
    {
        #if 0
        int len = recv(*sock_fd, buf,65535,0);
        if(len == -1)
        {
            perror("call to recv");
            exit(1);
        }
        fwrite(buf, 1, len ,fp);
        #endif

        //inet_ntop(AF_INET,&pin.sin_addr,host_name,sizeof(host_name));  
        //printf("received from client(%s):%s\n",host_name,buf);  

        #if 1
        if(idx == 0)
        {
            if(send(*sock_fd,create_json,strlen(create_json)+1,0) == -1)  
            {  
                perror("call to send");  
                exit(1);  
            }
            printf("================\n%s\n",create_json);
        }
        else if(1 && idx == 1)
        {
            ret = send(*sock_fd,join_00_json,strlen(join_00_json)+1,0);
            printf("========%d========\n%s\n",ret,join_00_json);
        }
        else if(0 && idx == 2)
        {
            ret = send(*sock_fd,leave_json,strlen(leave_json)+1,0);
            printf("=======%d=========\n%s\n",ret,leave_json);
        }
        else if(0 && idx == 3600)
        {
            printf("time arraive,send stop json\n");
            ret = send(*sock_fd,stop_json,strlen(stop_json)+1,0);
            printf("========%d========\n%s\n",ret,stop_json);
        }
        else if(0 && idx == 4)
        {
            if(send(*sock_fd,create_01_json,strlen(create_01_json)+1,0) == -1)  
            {  
                perror("call to send");  
                exit(1);  
            }
            printf("================\n%s\n",create_01_json);
        }
        else if(1 && idx == 5)
        {
            ret = send(*sock_fd,join_01_json,strlen(join_01_json)+1,0);
            printf("======%d==========\n%s\n",ret,join_01_json);
        }
        else if(1 && idx == 6)
        {
            ret = send(*sock_fd,join_02_json,strlen(join_02_json)+1,0);
            printf("======%d==========\n%s\n",ret,join_02_json);
        }
        else if(1 && idx == 7)
        {
            ret = send(*sock_fd,join_03_json,strlen(join_03_json)+1,0);
            printf("======%d==========\n%s\n",ret,join_03_json);
        }
        else if(1 && idx == 8)
        {
            ret = send(*sock_fd,join_04_json,strlen(join_04_json)+1,0);
            printf("======%d==========\n%s\n",ret,join_04_json);
        }
        else if(1 && idx == 9)
        {
            ret = send(*sock_fd,join_05_json,strlen(join_05_json)+1,0);
            printf("======%d==========\n%s\n",ret,join_05_json);
        }
        else if(1 && idx == 10)
        {
            usleep(1000*1000*10);
            ret = send(*sock_fd,videomix_json,strlen(videomix_json)+1,0);
            printf("======%d==========\n%s\n",ret,videomix_json);
        }
        else if(1 && idx == 11)
        {
            ret = send(*sock_fd,audiomix_json,strlen(audiomix_json)+1,0);
            printf("======%d==========\n%s\n",ret,audiomix_json);
        }
        else if(1 && idx == 12)
        {
            usleep(1000*1000*5);
            ret = send(*sock_fd,record_json,strlen(record_json)+1,0);
            printf("======%d==========\n%s\n",ret,record_json);
        }
        else if(0 && idx == 13)
        {
            //usleep(1000*500);
            ret = send(*sock_fd,pause_record_json,strlen(pause_record_json)+1,0);
            printf("======%d==========\n%s\n",ret,pause_record_json);
        }
        else if(0 && idx == 14)
        {
            //usleep(1000*500);
            ret = send(*sock_fd,resume_record_json,strlen(resume_record_json)+1,0);
            printf("======%d==========\n%s\n",ret,resume_record_json);
        }
        else if(1 && idx == 15)
        {
            //usleep(1000*1000 * 5400); //20s
            sleep(60*0.3); //20s
            ret = send(*sock_fd,stop_record_json,strlen(stop_record_json)+1,0);
            printf("======%d==========\n%s\n",ret,stop_record_json);
        }
        
        idx++;
        usleep(100);
        //printf("idx=%d\n",idx);
        #endif
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
    setsockopt(sock_descriptor,IPPROTO_TCP,TCP_NODELAY,(char*)&nZero,sizeof(int));
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

