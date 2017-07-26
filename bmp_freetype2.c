#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <wchar.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef long LONG;

#pragma pack (2)
typedef struct tagBITMAPFILEHEADER{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
}BITMAPINFOHEADER;

typedef struct tagRGBQUAD{
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
}RGBQUAD;

typedef struct tagIMAGEDATA{
    BYTE blue;
    BYTE green;
    BYTE red;
}IMAGEDATA;


BITMAPFILEHEADER strHead;
BITMAPINFOHEADER strInfo;
IMAGEDATA *arrayColor;
LONG nWidth;
LONG nHeight;

void showBmpHead(BITMAPFILEHEADER head)
{
    printf("bmp header:\n");
    printf("bfType:%0x\n",head.bfType);
    printf("size:%d\n",head.bfSize);
    printf("offsetSize:%d\n",head.bfOffBits);
}

void showBmpInfoHead(BITMAPINFOHEADER infoHead)
{
    printf("bmp info header:\n");
    printf("bmp width:%d\n",infoHead.biWidth);
    printf("bmp height:%d\n",infoHead.biHeight);
}

int WordInsert2Bmp(char *strFile)
{
    FILE *fpi,*fpw;
    fpi = fopen(strFile,"rb");
    FT_Library pFTLib = NULL;
    FT_Face pFTFace = NULL;
    FT_Error error = 0;
    error = FT_Init_FreeType(&pFTLib);
    if(error){
        pFTLib = 0;
        printf("There is some error when init library\n");
        return -1;
    }

    error = FT_New_Face(pFTLib, "/home/topsluo/Russo_One.ttf", 0, &pFTFace);
    if(error){
        printf("Open font failed\n");
        return -1;
    }

    FT_Set_Char_Size(pFTFace, 0, 16*64, 300, 300);
    FT_Glyph glyph;
    wchar_t *wszString = L"TEST    BY    TOPSLUO";
    WORD word;
    if(fpi != NULL){
        fread(&strHead, 1, sizeof(struct tagBITMAPFILEHEADER), fpi);
        if(0x4D42 != strHead.bfType)
        {
            printf("The file is not a bmp file\n");
            return -1;
        }
        
        showBmpHead(strHead);
        fread(&strInfo, 1, sizeof(struct tagBITMAPINFOHEADER), fpi);
        showBmpInfoHead(strInfo);
        nWidth = strInfo.biWidth;
        nHeight = strInfo.biHeight;
        DWORD size = nWidth * nHeight;
        arrayColor = (IMAGEDATA *)malloc(nWidth * nHeight * sizeof(struct tagIMAGEDATA));
        fread(arrayColor, 1, nWidth*nHeight*sizeof(struct tagIMAGEDATA), fpi);
        int bitmap_width_sum = 0;
        int wszStringLen = wcslen(wszString)+1;
        printf("wszStringLen: %d\n",wszStringLen);
        for(int k=0; k < wszStringLen; k++)
        {
            memcpy(&word, wszString+k, 2);
            FT_Load_Glyph(pFTFace, FT_Get_Char_Index(pFTFace,word), FT_LOAD_DEFAULT);
            error = FT_Get_Glyph(pFTFace->glyph, &glyph);
            FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
            FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
            FT_Bitmap  *bitmap = &bitmap_glyph->bitmap;
            for(int i=0;i<bitmap->rows;++i)
            {
                for(int j=0;j<bitmap->width;++j)
                {
                    if(bitmap->buffer[i * bitmap->width +j] != 0)
                    {
                        //arrayColor[(i+100)*strInfo.biWidth+j+bitmap_width_sum+100].green = 0;
                        //arrayColor[(i+100)*strInfo.biWidth+j+bitmap_width_sum+100].blue = 0;
                    	arrayColor[(strInfo.biHeight-i)*strInfo.biWidth+j+bitmap_width_sum].green = 0;
                    	arrayColor[(strInfo.biHeight-i)*strInfo.biWidth+j+bitmap_width_sum].blue = 0;
                    }
                }
            }
            bitmap_width_sum += bitmap->width + 10;
        }
        FT_Done_Glyph(glyph);
        glyph = NULL;
        FT_Done_Face(pFTFace);
        pFTFace = NULL;
        FT_Done_FreeType(pFTLib);
        pFTLib = NULL;
        fclose(fpi);
    }
    else{
        printf("file open error\n");
        return -1;
    }

    if((fpw=fopen("b.bmp","wb")) == NULL){
        printf("Create the bmp file error\n");
        return -1;
    }
    fwrite(&strHead,1,sizeof(struct tagBITMAPFILEHEADER),fpw);
    strInfo.biHeight = strInfo.biHeight;
    fwrite(&strInfo, 1, sizeof(struct tagBITMAPINFOHEADER), fpw);
    fwrite(arrayColor,1, nWidth*nHeight*sizeof(struct tagIMAGEDATA),fpw);
    fclose(fpw);
    return 0;
}

int main()
{
    printf("%d\n",sizeof(struct tagBITMAPFILEHEADER));
    printf("%d\n",sizeof(struct tagBITMAPINFOHEADER));
    WordInsert2Bmp("canvas.bmp");
    return 0;
}
