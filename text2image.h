#ifndef __TEXT2IMAGE_H__
#define __TEXT2IMAGE_H__
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <wchar.h>
#include <iconv.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H


struct MyBitmap{
    unsigned int counter;
    unsigned int origin;

    struct Bitmap{
        unsigned int width;
        unsigned int height;
        unsigned int horiBearingX;
        unsigned int horiBearingY;
        unsigned int horiAdvance;
    }bitmap[256];
};

class TextToImageFT
{
public:
    TextToImageFT(void);
    virtual ~TextToImageFT(void);
    int InitFT(const char * fname, unsigned int nHeight);
    int FreeFT();
    int PasteImage(int xOffset,int yOffset,int pitch,int width,int height,unsigned char * buff);
    int CalcTextImageSize(int *width,int *height,const wchar_t * text);
    int DrawText(const wchar_t * text);
    int GetRGBImage(int *width,int *height,unsigned char ** rgbBuff);
    int GetYUV420Image(int *width,int *height,unsigned char ** yuvBuff);
    int SaveRGBToBMPFile(unsigned char *image, int xsize, int ysize, char *filename);
    int rgb24_to_yuv420p(unsigned char *pY,unsigned char *pU, unsigned char *pV, unsigned char *pRGBBuffer, int width,int height);

protected:
    unsigned char *m_rgb24Buff;
    unsigned char *expanded_data;
    unsigned int nWidth;
    unsigned int nHeight;
    unsigned int ftHeiht;
    FT_Face ftFace;
    FT_Library ftLibrary;
    MyBitmap mybitmap;
};

#endif
