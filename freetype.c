#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ft2build.h"
#include <wchar.h>
#include FT_FREETYPE_H
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef long LONG;
typedef struct tagBITMAPFILEHEADER{
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
    LONG biXpelsPerMeter;
    LONG biYpelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
}BITMAPINFOHEADER;

typedef struct tagIMAGEDATA
{
    BYTE blue;
    BYTE green;
    BYTE red;
}IMAGEDATA;

BITMAPFILEHEADER strHead;
BITMAPINFOHEADER strInfo;
LONG nWidth;
LONG nHeight;

int WordInsertToBmp(char *strFile)
{
    FILE *fp_in, *fp_out;
    FT_Library pFTLib = NULL;
    FT_Face pFTFace = NULL;
    FT_Error error = 0;

    error = FT_Init_FreeType(&pFTLib);
    if(error)
    {
        printf("There is some error when init library\n");
        return -1;
    }

    error = FT_New_Face(pFTLib, "/home/b1shen/Russo_One.ttf", 0, &pFTFace);
    if(error)
    {
        printf("Open ttf file failed\n");
        return -1;
    }

    error = FT_Set_Char_Size(pFTFace, 0, 16*64, 300, 300);
    if(error)
    {
        printf("Set char size failed\n");
        return -1;
    }

    FT_Glyph glyph;
}
int main(int argc, char **argv)
{
    FT_Library      font_library;
    FT_Face         font_face;
    FT_Bitmap       bitmap;
    FT_GlyphSlot    cur_glyph;
    FT_Glyph_Metrics glyph_metrics;

    int glyph_ind;
    int num_chars;
    char char_name[256];
    if(argc != 2)
        exit(1);
    if(FT_Init_FreeType(&font_library))
        exit(1);
    if(FT_New_Face(font_library, argv[1], 0, &font_face))
        exit(1);
    if(FT_Set_Char_Size(font_face, 0, 768, 300, 300))
        exit(1);

    num_chars = (int)font_face->num_glyphs;
    FT_Set_Transform(font_face, NULL, NULL);

    for(glyph_ind = 0; glyph_ind < num_chars; glyph_ind++)
    {
        if( FT_Load_Glyph(font_face, glyph_ind, FT_LOAD_DEFAULT) )
            exit(1);
        cur_glyph = font_face->glyph;
        if(cur_glyph->format != FT_GLYPH_FORMAT_BITMAP)
        {
            if(FT_Render_Glyph(font_face->glyph, FT_RENDER_MODE_MONO))
                exit(1);
        }
        if( FT_Get_Glyph_Name(font_face, glyph_ind, char_name, 16) )
            exit(1);

        bitmap = cur_glyph->bitmap;
        glyph_metrics = cur_glyph->metrics;

        printf("Glyph %d name %s %ld %ld %ld %d %d\n",
        glyph_ind,
        char_name,
        glyph_metrics.horiBearingX / 64,
        glyph_metrics.horiBearingY / 64,
        glyph_metrics.horiAdvance / 64,
        bitmap.width, bitmap.rows);
    }

    return 0;
}
