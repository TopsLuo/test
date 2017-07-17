#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/************************************************
For n-bit sampling audio signal,If both A and B 
are negative Y = A * B - (A * B / (-(pow(2, n-1) -1)))
else         Y = A * B - (A * B / (POW(2,n-1)))
************************************************/
float Mix_001(const float *a, const float *b)
{
    float c;
    if(*a < 0 && *b < 0)
        c = *a + *b - ((*a) * (*b) / -(pow(2,32-1) -1));
    else
        c = *a + *b - ((*a) * (*b) / (pow(2,32-1)));

    c = c > 1 ? 1 : c;
    c = c < -1 ? -1 : c;
    return c;
}

/****************************
*线性叠加后求平均值
*优点：不会产生溢出，噪音较小
*缺点:衰减过大，影响通话质量
****************************/
float Mix_002(const float *a, const float *b)
{
    return (*a + *b) / 2;
}

int main(int argc, char **argv)
{
    /*仅支持音频数据为FLTP类型，即一个采样点占4个字节（32bits）*/
    if(argc < 4)
    {
        fprintf(stderr,"Support fltp format,Use it like this:\n");
        fprintf(stderr,"    %s in_1.pcm in_2.pcm out.pcm\n",argv[0]);
        return 0;
    }
    FILE *in_0 = fopen(argv[1],"rb");
    FILE *in_1 = fopen(argv[2],"rb");
    FILE *out = fopen(argv[3],"wb");

    char buf_0[4];
    char buf_1[4];
    while(1)
    {
        //读取一个声道的一个采样点
        int ret_0 = fread(buf_0,4,1,in_0);
        int ret_1 = fread(buf_1,4,1,in_1);
        printf("ret_0=%d,ret_1=%d\n",ret_0,ret_1);
        if(ret_0 == 0 || ret_1 == 0)
            break;

        //转换成float类型
        float *a = (float*)buf_0;
        float *b = (float*)buf_1;
        float c = Mix_001(a,b);
        //float c = Mix_002(a,b);

        fwrite(&c,4,1,out);
    }
}
