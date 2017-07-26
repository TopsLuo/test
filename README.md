  编译说明：
  =========
    1.  依赖库
        1.1 ffmpeg.3.2.2,编译ffmpeg前，需要先编译libx264，然后configure中打开libx264
        1.2 libx264,开源项目
        1.3 ptlib-2.0.1
        1.4 jsoncpp-1.8.0,直接编译这个可能会出问题，需要修改confiruge
        1.5 libxml2-2.7.8,开源项目
        1.6 freetype（目前还没用上，后面需要添加）
        1.7 glog,日志管理的库（google的开源项目）

    2.  编译
        2.1 需要把上面的库编译出来，并安装到指定目录。
            该步骤已经脚本化。在当前目录中有一个deps目录，进入该目录，执行./tarball.sh即可自动编译及安装
        2.2 回到当前目录，执行make即可编译出itcMCU可执行文件

    3.  ldd itcMCU找不到动态库问题
        3.1 确定动态库的目录
        3.2 执行sudo vim /etc/ld.so.conf，把动态库目录的绝对目录添加了该文件的末尾
        3.3 sudo /sbin/ldconfig，使修改生效

    4.  not found freetype2问题
        如果在编译deps目录下的第三方依赖库中的ffmpeg失败，可能是configure过程中出错，目前碰到的是"not found freetype2".
        原因：系统环境变量PKG_CONFIG_PATH为默认路径，而freetyp2是安装在用户的指定目录，所以需要指定一下freetype2的pkgconfig目录，
        命令如下:"export PKG_CONFIG_PATH=**/itc_mcu/deps/install/lib/pkgconfig"

  运行问题：
  =========
    1.  一定要有conf目录，且目录下要有配置文件server_config.xml配置文件
    2.  一定要有conf目录，且目录下要有配置文件canvas.png文件（用作画布）
    3.  如果是Debug模式，一定要有dataTestDir目录，用于存放接收、解码、编码、发送等各个环节的媒体数据
        ，以便验证数据的正确性


  测试代码模块介绍：
  ================
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

19  #bmp_freetype2.c
    #把文字写入到bmp文件中去

20  #text2image
    #该工程有三个文件，text2image.cxx text2image.h text2imageMain.cxx。
    #该工程主要作用：测试frfeetype2把文字转换成bmp图片
