/******************************
Date: 2017年 07月 12日 星期三 15:09:33 CST
Author: TopsLuo
Function: ffmpeg时间戳转换测试，微秒转换成90kHz的时间戳
*****************************/
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/time.h"
#include <sys/time.h>

int main()
{
    //avcodec_register_all();
    AVRational tb =(AVRational){1, 90000}; 

    /*
    av_gettime_relative()获取的时间单位：微秒(us)
    其它时间：秒（s),毫秒(ms),纳秒（ns）转换关系:
        1s  = 1000ms
        1ms = 1000us
        1us = 1000ns
    */
    int64_t start = av_gettime_relative();
    int i;
    for(i = 0; i < 100; i++)
    {
        av_usleep(1000 *40);
        int64_t now = av_gettime_relative() - start;
        int64_t pts = av_rescale_q(now , AV_TIME_BASE_Q, tb);
        printf("now: %"PRId64" -> pts: %"PRId64"\n",now, pts);
    }
    return 0;
}
