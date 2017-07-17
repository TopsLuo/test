1.  #aacCodec.c
    测试aac转码，把aac转换成另外参数的aac

2.  #client.c
    模拟miniCC向媒体服务器推流，并接收媒体服务器发送过来的数据    

3.  #codec.c
    测试解码264裸流代码，把264的裸流解码成YUV420P 

4.  #parse_json.cpp
5.  #create_json.cpp
    json格式的解析及创建

6.  #list.cxx
    C++的list使用，主要为了测试解码后的YUV/PCM数据存储在list中，及解决闪屏的BUG

7.  #map.cxx
    C++的map使用，复习map的使用方法

8.  #server.c
    模拟业务服务器，主要模拟向媒体服务器发送create/join/leave/stop等json数据包,这些Jsoin数据包已经在程序里写死

8.1.  #epollserver.c
    模拟业务服务器，主要模拟向媒体服务器发送create/join/leave/stop等json数据包,但与server.c不同的是：epollserver.c接收外部传入的create/join/leavel/stop等命令，然后转发给媒体服务器(与tcp_json目录中的脚本配合使用)
    
8.2.  #tcp_json目录
    向epollserver发送create/join/leave/stop等json数据包，控制媒体服务器

9.  #time.c
    ffmpeg内部时间戳转换测试
    
10. #time_spend.cxx
    函数耗时的测试方法
    
11. #resample_mix.c
    #测试音频的重采样及混音

12. #test.264
    264的裸流，供codec.c使用

12. #test.aac
    aac音频数据，供aacCodec.c使用

13  #test.ts
    ts文件，供client.c使用

14  #filter_audio.c
    利用filter对音频进行重采样测试代码

15  #mixaudio.c
    #a_fltp_32it_LittleEndian_stereo.pcm
    #b_fltp_32it_LittleEndian_stereo.pcm
    音频混合测试代码及pcm数据

16  #layout.cxx
    #测试视频合并的布局
    #配合脚本layout.sh一起使用更佳

17  #transcoding.c
    #简单的转码器，为实现recorder模块铺路
    #添加的视频的filter，支持帧率、分辨率等的设置

18  #glog.cxx
    #测试google的日志管理（glog）
