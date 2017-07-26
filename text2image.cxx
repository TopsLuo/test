#include "text2image.h"

TextToImageFT::TextToImageFT(void)
{
    m_rgb24Buff = NULL;
    expanded_data = NULL; 
    memset(&mybitmap, 0,sizeof(MyBitmap));
}

TextToImageFT::~TextToImageFT(void)
{
    if(m_rgb24Buff)
    {
        free(m_rgb24Buff);
        m_rgb24Buff = NULL;
    }

    if(expanded_data)
    {
        free(expanded_data); 
        expanded_data = NULL;
    }

    FreeFT();
}

int TextToImageFT::InitFT(const char * fname, unsigned int nHeight)
{
    ftHeiht = nHeight;
    //初始化FreeType库..
    if (FT_Init_FreeType( &ftLibrary ))
    {
        printf("FT_Init_FreeType failed");
        return -1;
    }

    //加载一个字体,取默认的Face,一般为Regualer
    if (FT_New_Face( ftLibrary, fname, 0, &ftFace))
    {
        printf("FT_New_Face failed (there is probably a problem with your font file)");
        return -1;
    }

    //大小要乘64.这是规定。照做就可以了。
    FT_Set_Char_Size(ftFace,ftHeiht << 6, ftHeiht << 6, 96, 96);

#if 0
    FT_Matrix matrix;

    FT_Vector pen;
    //给它设置个旋转矩阵
    float angle = (float) -0/180.* 3.14;
    matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
    matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
    matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
    matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );
    FT_Set_Transform( ftFace, &matrix, &pen );
#endif

    return 0;
}


int TextToImageFT::FreeFT()
{
    FT_Done_Face(ftFace);
    FT_Done_FreeType(ftLibrary);
    return 0;
}

int TextToImageFT::PasteImage(int xOffset,int yOffset,int pitch,int width,int height,unsigned char * buff)
{ 
    if(expanded_data)
        free(expanded_data);   

    expanded_data = new unsigned char[ 3 * pitch * height];
    memset(expanded_data, 0x00, 3 * pitch * height);

    for(int j=0; j < height ; j++)
    {
        for(int i=0; i < width; i++)
        {   
            // expanded_data[3*(i+(height-j-1)*pitch)]=
            // expanded_data[3*(i+(height-j-1)*pitch)+1] =
            // expanded_data[3*(i+(height-j-1)*pitch)+2] = buff[i + width*j];

            expanded_data[3*(i+j*pitch)]=
            expanded_data[3*(i+j*pitch)+1] =
            expanded_data[3*(i+j*pitch)+2] = buff[i + width*j];
            //(i>=bitmap.width || j>=bitmap.rows) ? 0 : bitmap.buffer[i + bitmap.width*j];

        }
    }

    for (int j = 0/*yOffset*/; j < height; j++)
    {
        memcpy(m_rgb24Buff+(nWidth*(yOffset+j)+xOffset)*3,expanded_data+pitch*j*3,pitch*3);
    }

    return 0;
}

int TextToImageFT::CalcTextImageSize(int *width,int *height,const wchar_t * text)
{
    int use_kerning;
    int pen_x, pen_y;
    int start_x, end_x;
    int chr;
    FT_UInt prev_index;

    nWidth = 0;
    nHeight = 0;
    start_x = 0;
    end_x = 0;
    pen_x = 0;
    pen_y = 0;
    use_kerning = FT_HAS_KERNING(ftFace);
    prev_index = 0;
    memset(&mybitmap, 0,sizeof(MyBitmap));

    for (chr = 0; text[chr];chr++)
    {
        FT_UInt             index; 

        index = FT_Get_Char_Index(ftFace, text[chr]);

        if(FT_Load_Glyph(ftFace,index, FT_LOAD_NO_BITMAP))
            printf("FT_Load_Glyph failed");

        FT_Glyph glyph;

        if(FT_Get_Glyph(ftFace->glyph, &glyph ))
            printf("FT_Get_Glyph failed");

        FT_Render_Glyph(ftFace->glyph, FT_RENDER_MODE_NORMAL );
        FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
        FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

        //取道位图数据
        FT_Bitmap& bitmap = bitmap_glyph->bitmap;
        FT_Glyph_Metrics metrics = ftFace->glyph->metrics;

        int pitch = bitmap.pitch + 4 -1;
        pitch -= pitch%4;
        nWidth  += pitch;

        if(bitmap.pitch == 0)//空格
        {
            nWidth += (metrics.horiAdvance>>6);
        }

        if(nHeight < bitmap.rows)
        {
            nHeight = bitmap.rows; 
        }

        mybitmap.bitmap[mybitmap.counter].width = bitmap.pitch;
        mybitmap.bitmap[mybitmap.counter].height = bitmap.rows;
        mybitmap.bitmap[mybitmap.counter].horiBearingX = metrics.horiBearingX >> 6;
        mybitmap.bitmap[mybitmap.counter].horiBearingY = metrics.horiBearingY >> 6;
        mybitmap.bitmap[mybitmap.counter].horiAdvance = metrics.horiAdvance >> 6;

        //找到原始点的坐标
        if(mybitmap.origin < (metrics.horiBearingY>>6))
        {
            mybitmap.origin = metrics.horiBearingY>>6;
        }

        mybitmap.counter++;
        //  free glyph
        FT_Done_Glyph(glyph);
        glyph  =  NULL;
    }
    
    int len = 0;
    for(int i=0; i <mybitmap.counter; i++)
    {
        /*printf("width:%d,height:%d,",
                mybitmap.bitmap[i].width,
                mybitmap.bitmap[i].height
                );

        printf("horiBearingX:%d,horiBearingY:%d,horiAdvance:%d\n",
                mybitmap.bitmap[i].horiBearingX,
                mybitmap.bitmap[i].horiBearingY,
                mybitmap.bitmap[i].horiAdvance
                );*/
        if((mybitmap.origin - mybitmap.bitmap[i].horiBearingY + mybitmap.bitmap[i].height) > nHeight)
        {
            nHeight = mybitmap.origin - mybitmap.bitmap[i].horiBearingY + mybitmap.bitmap[i].height;
        }
    }

    if(nHeight%2)
        nHeight++;

    if (width)
        *width = nWidth;

    if (height)
        *height = nHeight;

    printf("nHeight: %d, nWidth: %d, origin:%d\n", nHeight,nWidth, mybitmap.origin);
    return 0;
}

int TextToImageFT::DrawText(const wchar_t * text)
{
    int width = 0;
    int height  =0;
    int xOffset = 0;
    int yOffset = 0;
    FT_GlyphSlot slot;
    unsigned int ft_previous = 0;


    CalcTextImageSize(&width,&height,text);


    if(m_rgb24Buff)
    {
        free(m_rgb24Buff);
        m_rgb24Buff = NULL;
    }
    
    m_rgb24Buff = (unsigned char*)malloc(nWidth*nHeight*3);
    memset(m_rgb24Buff,0,nWidth*nHeight*3);

    for (int chr = 0; text[chr];chr++)
    {
        FT_UInt             index; 
        index = FT_Get_Char_Index(ftFace, text[chr]);
        if(ft_previous)
        {
            FT_Vector delta;
            FT_Get_Kerning(ftFace, ft_previous, index, FT_KERNING_DEFAULT, &delta);
            //printf("delta.x:%d,delta.y:%d\n",delta.x >> 6, delta.y);
        }

        if(FT_Load_Glyph(ftFace, index, FT_LOAD_DEFAULT))
            printf("FT_Load_Glyph failed");

        FT_Glyph glyph;
        if(FT_Get_Glyph(ftFace->glyph, &glyph ))
            printf("FT_Get_Glyph failed");

        FT_Render_Glyph(ftFace->glyph, FT_RENDER_MODE_NORMAL  );
        FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1 );
        FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
        //取道位图数据
        FT_Bitmap& bitmap=bitmap_glyph->bitmap; 
        slot = ftFace->glyph;

        FT_Glyph_Metrics metrics = slot->metrics;
        int pitch = bitmap.pitch + 4 -1;
        pitch -= pitch%4;

        //yOffset = nHeight - bitmap.rows;
        yOffset = mybitmap.origin - (metrics.horiBearingY>>6);

        if(bitmap.width == 0 && bitmap.rows== 0)
        {
            //空格
            xOffset += (metrics.horiAdvance>>6);
        }
        PasteImage(xOffset, yOffset, pitch, bitmap.width, bitmap.rows, bitmap.buffer);
        xOffset += (pitch);

        //  free glyph
        FT_Done_Glyph(glyph);
        glyph  =  NULL;
        ft_previous = index;
    }

    return 0;
}


int TextToImageFT::GetRGBImage(int *width,int *height,unsigned char ** rgbBuff)
{
    *width = nWidth;
    *height = nHeight;
    *rgbBuff = m_rgb24Buff;
    return 0;
}

int TextToImageFT::GetYUV420Image(int *width,int *height,unsigned char ** yuvBuff)
{
    unsigned char *yuv = NULL;
    *width =nWidth;
    *height=nHeight;

    yuv =(unsigned char*) malloc(nWidth*nHeight*3/2);
    rgb24_to_yuv420p(yuv,yuv+nWidth*nHeight,yuv+nWidth*nHeight*5/4,m_rgb24Buff,nWidth,nHeight);
    *yuvBuff = yuv;
    return 0;
}

#define SCALEBITS 8
#define ONE_HALF (1 << (SCALEBITS - 1))
#define FIX(x) ((int)((x) * (1L<<SCALEBITS)+0.5))

int TextToImageFT::rgb24_to_yuv420p(unsigned char *pY,unsigned char *pU, unsigned char *pV, unsigned char *pRGBBuffer, int width,int height)
{
    int wrap, wrap3, x, y;
    int r, g, b, r1, g1, b1;
    unsigned char *p;

    wrap = width;
    wrap3 = width * 3;
    p = pRGBBuffer;
    for(y=0;y<height;y+=2) {
        for(x=0;x<width;x+=2) {
            r = p[0];
            g = p[1];
            b = p[2];
            r1 = r;
            g1 = g;
            b1 = b;
            pY[0] = (FIX(0.29900) * r + FIX(0.58700) * g +
                      FIX(0.11400) * b + ONE_HALF) >> SCALEBITS;
            r = p[3];
            g = p[4];
            b = p[5];
            r1 += r;
            g1 += g;
            b1 += b;
            pY[1] = (FIX(0.29900) * r + FIX(0.58700) * g +
                      FIX(0.11400) * b + ONE_HALF) >> SCALEBITS;
            p += wrap3;
            pY += wrap;

            r = p[0];
            g = p[1];
            b = p[2];
            r1 += r;
            g1 += g;
            b1 += b;
            pY[0] = (FIX(0.29900) * r + FIX(0.58700) * g +
                      FIX(0.11400) * b + ONE_HALF) >> SCALEBITS;
            r = p[3];
            g = p[4];
            b = p[5];
            r1 += r;
            g1 += g;
            b1 += b;
            pY[1] = (FIX(0.29900) * r + FIX(0.58700) * g +
                      FIX(0.11400) * b + ONE_HALF) >> SCALEBITS;

            pU[0] = ((- FIX(0.16874) * r1 - FIX(0.33126) * g1 +
                      FIX(0.50000) * b1 + 4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128;
            pV[0] = ((FIX(0.50000) * r1 - FIX(0.41869) * g1 -
                     FIX(0.08131) * b1 + 4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128;

            pU++;
            pV++;
            p += -wrap3 + 2 * 3;
            pY += -wrap + 2;
        }
        p += wrap3;
        pY += wrap;
    }
}

int TextToImageFT::SaveRGBToBMPFile(unsigned char *image, int xsize, int ysize, char *filename)
{
    unsigned char header[54] =
    {
        0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0,
        54, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0
    };
    
    long file_size = (long)xsize * (long)ysize * 3 + 54;
    header[2] = (unsigned char)(file_size &0x000000ff);
    header[3] = (file_size >> 8) & 0x000000ff;
    header[4] = (file_size >> 16) & 0x000000ff;
    header[5] = (file_size >> 24) & 0x000000ff;
    long width = xsize;
    header[18] = width & 0x000000ff;
    header[19] = (width >> 8) &0x000000ff;
    header[20] = (width >> 16) &0x000000ff;
    header[21] = (width >> 24) &0x000000ff;
    long height = -ysize; //数据倒着填充的
    header[22] = height &0x000000ff;
    header[23] = (height >> 24) &0x000000ff;
    header[24] = (height >> 16) &0x000000ff;
    header[25] = (height >> 8) &0x000000ff;

    FILE *fp;
    if (!(fp = fopen(filename, "wb")))
        return -1;

    fwrite(header, sizeof(unsigned char), 54, fp);
    fwrite(image, sizeof(unsigned char), (size_t)(long)xsize * ysize * 3, fp);
    fclose(fp);
    fp = NULL;
    return 0;
}
