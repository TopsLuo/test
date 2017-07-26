/*
Data:   
    2017年 07月 25日 星期二 17:36:29 CST
Author: 
    TopsLuo
Function:
    测试freetype2字符转图片功能，及对齐功能,支持中文字符，如果
    测试过程中发现中文字符无法转换成图片，请注意ttf字符库是否支持
    中文字符(Russo_One.ttf不支持中文字符)
*/
#include "text2image.h"
using namespace std;

int UTF8ToUnicode(const char *pmbs, wchar_t *pwcs, int size)
{
    int cnt = 0;
    // 这里 size-- 是预先除去尾零所需位置
    if (pmbs != NULL && pwcs != NULL && size-- > 0) {
        while (*pmbs != 0 && size > 0) {
            unsigned char ch = *pmbs;
            if (ch > 0x7FU) {
                int cwch = 0;
                while (ch & 0x80U) {
                    ch <<= 1;
                    cwch++;
                }
                *pwcs = *pmbs++ & (0xFFU >> cwch);
                while (--cwch > 0) {
                    *pwcs <<= 6;
                    *pwcs |= (*pmbs++ & 0x3FU);
                }
            } else {
                *pwcs = *pmbs++;
            }
            pwcs++;
            size--;
            cnt++;
        }
        *pwcs = 0;
        cnt++;
    }
    return cnt;
}
 
int UnicodeToUTF8(const wchar_t *pwcs, char *pmbs, int size)
{
    int cnt = 0;
    // 这里 size-- 是预先除去尾零所需位置
    if (pwcs != NULL && pmbs != NULL && size-- > 0) {
        while (*pwcs != 0 && size > 0) {
            if (*pwcs < 0x00000080U) {
                *pmbs++ = (char)*pwcs;
                size -= 1;
                cnt += 1;
            } else if (*pwcs < 0x00000800U) {
                // 剩余空间不够存放该字符
                if (size < 2) {
                    break;
                }
                *pmbs++ = (0xFFU << 6) | (*pwcs >> 6);
                *pmbs++ = 0x80U | (*pwcs & 0x3FU);
                size -= 2;
                cnt += 2;
            } else if (*pwcs < 0x00010000U) {
                // 剩余空间不够存放该字符
                if (size < 3) {
                    break;
                }
                *pmbs++ = (0xFFU << 5) | (*pwcs >> 12);
                *pmbs++ = 0x80U | ((*pwcs >> 6) & 0x3FU);
                *pmbs++ = 0x80U | (*pwcs & 0x3FU);
                size -= 3;
                cnt += 3;
            } else if (*pwcs < 0x00200000U) {
                // 剩余空间不够存放该字符
                if (size < 4) {
                    break;
                }
                *pmbs++ = (0xFFU << 4) | (*pwcs >> 18);
                *pmbs++ = 0x80U | ((*pwcs >> 12) & 0x3FU);
                *pmbs++ = 0x80U | ((*pwcs >> 6) & 0x3FU);
                *pmbs++ = 0x80U | (*pwcs & 0x3FU);
                size -= 4;
                cnt += 4;
            } else if (*pwcs < 0x04000000U) {
                // 剩余空间不够存放该字符
                if (size < 5) {
                    break;
                }
                *pmbs++ = (0xFFU << 3) | (*pwcs >> 24);
                *pmbs++ = 0x80U | ((*pwcs >> 18) & 0x3FU);
                *pmbs++ = 0x80U | ((*pwcs >> 12) & 0x3FU);
                *pmbs++ = 0x80U | ((*pwcs >> 6) & 0x3FU);
                *pmbs++ = 0x80U | (*pwcs & 0x3FU);
                size -= 5;
                cnt += 5;
            } else if (*pwcs < 0x80000000U) {
                // 剩余空间不够存放该字符
                if (size < 6) {
                    break;
                }
                *pmbs++ = (0xFFU << 2) | (*pwcs >> 30);
                *pmbs++ = 0x80U | ((*pwcs >> 24) & 0x3FU);
                *pmbs++ = 0x80U | ((*pwcs >> 18) & 0x3FU);
                *pmbs++ = 0x80U | ((*pwcs >> 12) & 0x3FU);
                *pmbs++ = 0x80U | ((*pwcs >> 6) & 0x3FU);
                *pmbs++ = 0x80U | (*pwcs & 0x3FU);
                size -= 6;
                cnt += 6;
            } else {
                // 无法识别的 Unicode 字符
                break;
            }
            pwcs++;
        }
        *pmbs = 0;
        cnt++;
    }
    return cnt;
}

int main(int argc, char **argv)
{
    TextToImageFT haha;
    //haha.InitFT("../conf/Russo_One.ttf", 30);
    haha.InitFT("../conf/simsun.ttf", 30);
    //wchar_t *text = L"By TopsLuo";
    //char mbs1[256] = {0};
    //wchar_t wcs1[]=L"测试文字";
    //int ret1 = UnicodeToUTF8(wcs1, mbs1, sizeof(mbs1)/sizeof(char));
    if(argc != 2)
    {
        printf("%s <string>\n",argv[0]);
        return -1;
    }

    char *mbs2 = argv[1];
    wchar_t wcs2[256] = { 0 };
    int ret2 = UTF8ToUnicode(mbs2, wcs2, sizeof(wcs2)/sizeof(wchar_t));

    haha.DrawText(wcs2);
    int height, width;
    unsigned char *buffer;
    char *rgbFileName = "/tmp/haha.bmp";
    haha.GetRGBImage(&width, &height, &buffer);
    haha.SaveRGBToBMPFile(buffer, width, height, rgbFileName);
    int w,h;
    unsigned char *yuvbuf = NULL;
    haha.GetYUV420Image(&w, &h, &yuvbuf);
    char filename[256] = {0};
    sprintf(filename, "/tmp/%d_%d.yuv",w,h);
    FILE *fp = fopen(filename, "wb");
    if(fp)
    {
        fwrite(yuvbuf,1,(w*h*3)/2, fp);
        fclose(fp);
    }
    free(yuvbuf);

    return 0;
}
