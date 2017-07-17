#include <iostream>
#ifdef __cplusplus
extern "C"{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#ifdef __cplusplus
}
#endif
#include <string.h>
using namespace std;

#define WIDTH 1920
#define HEIGHT 1080

enum Layout_format{

    /*
    (0,0,1920,1080)
    */
    LAYOUT_01_1X1 = 0,

    /*
    (0,270,960,540),(960,270,960,540)
    */
    LAYOUT_02_2X1,

    /*
            (480,0,960,540)
    (0,540,960,540),(960,540,960,540)
    */
    LAYOUT_03_1_2,

    /*
    (0,0,960,540),  (960,0,960,540)
    (0,540,960,540),(960,540,960,540)
    */
    LAYOUT_04_2X2,

    /*
              (0,90,960,540),(960,90,960,540)
    (0,630,640,360),(640,630,640,360),(1280,630,640,360)
    */
    LAYOUT_05_2P3, 
    /*
    (0,0,640,360),  (640,0,640,360),(1280,0,640,360)
    (0,360,640,360),        (640,360,1280,720)
    (0,720,640,360),
    */
    LAYOUT_06_1P5,
    
    /*
    (0,180,640,360),(640,180,640,360),(1280,180,640,360) 
    (0,540,640,360),(640,540,640,360),(1280,540,640,360) 
    */
    LAYOUT_06_3X2,

    /*
            (320,0,640,360),  (960,0,640,360)
    (0,360,640,360),(640,360,640,360),(1280,360,640,360)
            (320,720,640,360),(960,720,640,360)
    */
    LAYOUT_07_2_3_2,

    /*
    (0,0,480,270),(480,0,480,270),(960,0,480,270),(1440,0,480,270)
    (0,270,480,270),
    (0,540,480,270),            (480,270,1440,810)
    (0,810,480,270),
    */
    LAYOUT_08_1P7,

    /*
    (0,0,640,360),  (640,0,640,360),  (1280,0,640,360)
    (0,360,640,360),(640,360,640,360),(1280,360,640,360)
            (320,720,640,360),(960,720,640,360)
    */
    LAYOUT_08_3_3_2,

    /*
    (0,0,640,360),  (640,0,640,360),  (1280,0,640,360)
    (0,360,640,360),(640,360,640,360),(1280,360,640,360)
    (0,720,640,360),(640,720,640,360),(1280,720,640,360)
    */
    LAYOUT_09_3X3,

    /*
    (0,0,384,216),  (384,0,384,216),(768,0,384,216),(1152,0,384,216),(1536,0,384,216)
    (0,216,384,216),
    (0,432,384,216),                        (384,216,1536,864)
    (0,648,384,216),
    (0,864,384,216),
    */
    LAYOUT_10_1P9,

    /*
    (0,0,480,270),  (480,0,480,270),  (960,0,480,270),  (1440,0,480,270)
                    (0,270,960,540),(960,270,960,540)
    (0,810,480,270),(480,810,480,270),(960,810,480,270),(1440,810,480,270)
    */
    LAYOUT_10_2P8,

    /*
                    (0,0,960,540),(960,0,960,540)
    (0,540,480,270),(480,540,480,270),(960,540,480,270),(1440,540,480,270)
    (0,810,480,270),(480,810,480,270),(960,810,480,270),(1440,810,480,270)
    */
    LAYOUT_10_2V8,

    /*
            (240,135,480,270),(720,135,480,270),(1200,135,480,270)
    (0,405,480,270),(480,405,480,270),(960,405,480,270),(1440,405,480,270)
            (240,675,480,270),(720,675,480,270),(1200,675,480,270)
    */
    LAYOUT_10_3_4_3,

    /*
    (0,0,480,360),  (480,0,480,360),  (960,0,480,360),  (1440,0,480,360)
    (0,360,480,360),        (480,360,960,720),          (1440,360,480,360)
    (0,720,480,360),(480,720,480,360),(960,720,480,360),(1440,720,480,360)
    */
    LAYOUT_11_1P10,

    //LAYOUT_11_3P2P6,

    /*
    (0,135,480,270),(480,135,480,270),(960,135,480,270),(1440,135,480,270)
            (240,405,480,270),(720,405,480,270),(1200,405,480,270)
    (0,675,480,270),(480,675,480,270),(960,675,480,270),(1440,675,480,270)
    */
    LAYOUT_11_4_3_4,

    /*
    (0,135,480,270),(480,135,480,270),(960,135,480,270),(1440,135,480,270)
    (0,405,480,270),(480,405,480,270),(960,405,480,270),(1440,405,480,270)
    (0,675,480,270),(480,675,480,270),(960,675,480,270),(1440,675,480,270)
    */
    LAYOUT_12_4X3,

    /*
    (0,0,384,216),  (384,0,384,216),  (768,0,384,216),  (1152,0,384,216),  (1536,0,384,216)
                            (0,216,960,648),(960,216,960,648)
    (0,864,384,216),(384,864,384,216),(768,864,384,216),(1152,864,384,216),(1536,864,384,216)
    */
    LAYOUT_12_2P10,

    /*
                            (0,0,960,648),(960,0,960,648)
    (0,648,384,216),(384,648,384,216),(768,648,384,216),(1152,648,384,216),(1536,648,384,216)
    (0,864,384,216),(384,864,384,216),(768,864,384,216),(1152,864,384,216),(1536,864,384,216)
    */
    LAYOUT_12_2V10,

    /*
    (0,0,320,180),(320,0,320,180),(640,0,320,180),(960,0,320,180),(1280,0,320,180),(1600,0,320,180)
    (0.180,320,180),
    (0.360,320,180),
    (0.540,320,180),                            (320,180,1600,900)
    (0.720,320,180),
    (0.900,320,180),
    */
    LAYOUT_12_1P11,

    /*
    (0,0,960,540),                    (960,0,480,270),  (1440,0,480,270)
                                      (960,270,480,270),(1440,270,480,270)
    (0,540,480,270),(480,540,480,270),(960,540,480,270),(1440,540,480,270)
    (0,810,480,270),(480,810,480,270),(960,810,480,270),(1440,810,480,270)
    */
    LAYOUT_13_1P12,

    /*
    (0,0,480,270),  (480,0,480,270),  (960,0,480,270),  (1440,0,480,270)
    (0,270,480,270),      (480,270,960,540),            (1440,270,480,270)
    (0,540,480,270),                                    (1440,540,480,270)
    (0,810,480,270),(480,810,480,270),(960,810,480,270),(1440,810,480,270)
    */
    LAYOUT_13_1C12,

    /*
                    (480,0,480,270),  (960,0,480,270)                   
    (0,270,480,270),(480,270,480,270),(960,270,480,270),(1440,270,480,270)
    (0,540,480,270),(480,540,480,270),(960,540,480,270),(1440,540,480,270)
            (240,810,480,270),(720,810,480,270),(1200,810,480,270)
    */
    LAYOUT_13_2_4_4_3,

    /*
            (135,0,480,270),  (615,0,480,270),  (1095,0,480,270)
    (0,270,480,270),(480,270,480,270),(960,270,480,270),(1440,270,480,270)
    (0,540,480,270),(480,540,480,270),(960,540,480,270),(1440,540,480,270)
            (240,810,480,270),(720,810,480,270),(1200,810,480,270)
    */
    LAYOUT_14_3_4_4_3,

    /*
                                (0,0,960,720),(960,0,960,720)
    (0,720,320,180),(320,720,320,180),(640,720,320,180),(960,720,320,180),(1280,720,320,180),(1600,720,320,180)
    (0,900,320,180),(320,900,320,180),(640,900,320,180),(960,900,320,180),(1280,900,320,180),(1600,900,320,180)
    */
    LAYOUT_14_2V12,

    /*
    (0,0,480,270),  (480,0,480,270),  (960,0,480,270),  (1440,0,480,270)
    (0,270,480,270),(480,270,480,270),(960,270,480,270),(1440,270,480,270)
    (0,540,480,270),(480,540,480,270),(960,540,480,270),(1440,540,480,270)
            (240,810,480,270),(720,810,480,270),(1200,810,480,270)
    */
    LAYOUT_15_4_4_4_3,

    /*
    (0,0,480,270),  (480,0,480,270),  (960,0,480,270),  (1440,0,480,270)
    (0,270,480,270),(480,270,480,270),(960,270,480,270),(1440,270,480,270)
    (0,540,480,270),(480,540,480,270),(960,540,480,270),(1440,540,480,270)
    (0,810,480,270),(480,810,480,270),(960,810,480,270),(1440,810,480,270)
    */
    LAYOUT_16_4X4,
    UNKNOW
};

struct Position{
    int posx;
    int posy;
    int width;
    int height;
};
typedef struct VideoMixPosition{
    char* layout_name;
    Layout_format format;
    int num;
    Position pn[16];//4x4
}VMP;

const VMP videomixpn[]={
    {"layout_01_1x1" , LAYOUT_01_1X1, 1,{
                {0, 0, 1920, 1080}
              }
    },
    {"layout_02_2x1", LAYOUT_02_2X1, 2, {
                {0, 270, 960, 540},
                {960, 270, 960, 540}
              }
    },
    {"layout_03_1_2", LAYOUT_03_1_2, 3, {
                {480, 0, 960, 540},
                {0, 540, 960, 540},
                {960, 540, 960, 540}
              }
    },
    {"layout_04_2x2", LAYOUT_04_2X2, 4, {
                {0, 0, 960, 540},
                {960, 0, 960, 540},
                {0, 540, 960, 540},
                {960, 540, 960, 540}
              }
    },
    {"layout_05_2P3", LAYOUT_05_2P3, 5, {
                {0, 90, 960, 540},
                {960, 90, 960, 540},
                {0, 630, 640, 360},
                {640, 630, 640, 360},
                {1280, 630, 640, 360}
              }
    },
    {"layout_06_1P5", LAYOUT_06_1P5, 6, {
                {0, 0, 640, 360},
                {640, 0, 640, 360},
                {1280, 0, 640, 360},
                {0, 360, 640, 360},
                {640, 360, 1280, 720},
                {0, 720, 640, 360}
              }
    },
    {"layout_06_3x2", LAYOUT_06_3X2, 6, {
                {0, 180, 640, 360},
                {640, 180, 640, 360},
                {1280, 180, 640, 360},
                {0, 540, 640, 360},
                {640, 540, 640, 360},
                {1280, 540, 640, 360}
              }
    },
    {"layout_07_2_3_2", LAYOUT_07_2_3_2, 7, {
                {320, 0, 640, 360},
                {960, 0, 640, 360},
                {0, 360, 640, 360},
                {640, 360, 640, 360},
                {1280, 360, 640, 360},
                {320, 720, 640, 360},
                {960, 720, 640, 360}
              }
    },
    {"layout_08_1P7", LAYOUT_08_1P7, 8, {
                {0, 0, 480, 270},
                {480, 0, 480, 270},
                {960, 0, 480, 270},
                {1440, 0, 480, 270},
                {0, 270, 480, 270},
                {480, 270, 1440, 810},
                {0, 540, 480, 270},
                {0, 810, 480, 270}
              }
    },
    {"layout_08_3_3_2", LAYOUT_08_3_3_2, 8, {
                {0, 0, 640, 360},
                {640, 0, 640, 360},
                {1280, 0, 640, 360},
                {0, 360, 640, 360},
                {640, 360, 640, 360},
                {1280, 360, 640, 360},
                {320, 720, 640, 360},
                {960, 720, 640, 360}
              }
    },
    {"layout_09_3x3", LAYOUT_09_3X3, 9, {
                {0, 0, 640, 360},
                {640, 0, 640, 360},
                {1280, 0, 640, 360},
                {0, 360, 640, 360},
                {640, 360, 640, 360},
                {1280, 360, 640, 360},
                {0, 720, 640, 360},
                {640, 720, 640, 360},
                {1280, 720, 640, 360}
              }
    },
    {"layout_10_1P9", LAYOUT_10_1P9, 10, {
                {0, 0, 384, 216},
                {384, 0, 384, 216},
                {768, 0, 384, 216},
                {1152, 0, 384, 216},
                {1536, 0, 384, 216},
                {0, 216, 384, 216},
                {384, 216, 1536, 864},
                {0, 432, 384, 216},
                {0, 648, 384, 216},
                {0, 864, 384, 216}
              }
    },
    {"layout_10_2P8", LAYOUT_10_2P8, 10, {
                {0, 0, 480, 270},
                {480, 0, 480, 270},
                {960, 0, 480, 270},
                {1440, 0, 480, 270},
                {0, 270, 960, 540},
                {960, 270, 960, 540},
                {0, 810, 480, 270},
                {480, 810, 480, 270},
                {960, 810, 480, 270},
                {1440, 810, 480, 270}
              }
    },
    {"layout_10_2V8", LAYOUT_10_2V8, 10, {
                {0, 0, 960, 540},
                {960, 0, 960, 540},
                {0, 540, 480, 270},
                {480, 540, 480, 270},
                {960, 540, 480, 270},
                {1440, 540, 480, 270},
                {0, 810, 480, 270},
                {480, 810, 480, 270},
                {960, 810, 480, 270},
                {1440, 810, 480, 270}
              }
    },
    {"layout_10_3_4_3", LAYOUT_10_3_4_3, 10, {
                {240, 135, 480, 270},
                {720, 135, 480, 270},
                {1200, 135, 480, 270},
                {0, 405, 480, 270},
                {480, 405, 480, 270},
                {960, 405, 480, 270},
                {1440, 405, 480, 270},
                {240, 675, 480, 270},
                {720, 675, 480, 270},
                {1200, 675, 480, 270}
              }
    },
    {"layout_11_1P10", LAYOUT_11_1P10, 11, {
                {0, 0, 480, 360},
                {480, 0, 480, 360},
                {960, 0, 480, 360},
                {1440, 0, 480, 360},
                {0, 360, 480, 360},
                {480, 360, 960, 720},
                {1440, 360, 480, 360},
                {0, 720, 480, 360},
                {480, 720, 480, 360},
                {960, 720, 480, 360},
                {1440, 720, 480, 360},
              }
    },
    {"layout_11_4_3_4", LAYOUT_11_4_3_4, 11, {
                {0, 135, 480, 270},
                {480, 135, 480, 270},
                {960, 135, 480, 270},
                {1440, 135, 480, 270},
                {240, 405, 480, 270},
                {720, 405, 480, 270},
                {1200, 405, 480, 270},
                {0, 675, 480, 270},
                {480, 675, 480, 270},
                {960, 675, 480, 270},
                {1440, 675, 480, 270}
              }
    },
    {"layout_12_4x3", LAYOUT_12_4X3, 12, {
                {0, 135, 480, 270},
                {480, 135, 480, 270},
                {960, 135, 480, 270},
                {1440, 135, 480, 270},
                {0, 405, 480, 270},
                {480, 405, 480, 270},
                {960, 405, 480, 270},
                {1440, 405, 480, 270},
                {0, 675, 480, 270},
                {480, 675, 480, 270},
                {960, 675, 480, 270},
                {1440, 675, 480, 270}
              }
    },
    {"layout_12_2P10", LAYOUT_12_2P10, 12, {
                {0, 0, 384, 216},
                {384, 0, 384, 216},
                {768, 0, 384, 216},
                {1152, 0, 384, 216},
                {1536, 0, 384, 216},
                {0, 216, 960, 648},
                {960, 216, 960, 648},
                {0, 864, 384, 216},
                {384, 864, 384, 216},
                {768, 864, 384, 216},
                {1152, 864, 384, 216},
                {1536, 864, 384, 216}
              }
    },
    {"layout_12_2V10", LAYOUT_12_2V10, 12, {
                {0, 0, 960, 648},
                {960, 0, 960, 648},
                {0, 648, 384, 216},
                {384, 648, 384, 216},
                {768, 648, 384, 216},
                {1152, 648, 384, 216},
                {1536, 648, 384, 216},
                {0, 864, 384, 216},
                {384, 864, 384, 216},
                {768, 864, 384, 216},
                {1152, 864, 384, 216},
                {1536, 864, 384, 216}
              }
    },
    {"layout_12_1P11", LAYOUT_12_1P11, 12, {
                {0, 0, 320, 180},
                {320, 0, 320, 180},
                {640, 0, 320, 180},
                {960, 0, 320, 180},
                {1280, 0, 320, 180},
                {1600, 0, 320, 180},
                {0, 180, 320, 180},
                {320, 180, 1600, 900},
                {0, 360, 320, 180},
                {0, 540, 320, 180},
                {0, 720, 320, 180},
                {0, 900, 320, 180}
              }
    },
    {"layout_13_1P12", LAYOUT_13_1P12, 13, {
                {0, 0, 960, 540},
                {960, 0, 480, 270},
                {1440, 0, 480, 270},
                {960, 270, 480, 270},
                {1440, 270, 480, 270},
                {0, 540, 480, 270},
                {480, 540, 480, 270},
                {960, 540, 480, 270},
                {1440, 540, 480, 270},
                {0, 810, 480, 270},
                {480, 810, 480, 270},
                {960, 810, 480, 270},
                {1440, 810, 480, 270}
              }
    },
    {"layout_13_1C12", LAYOUT_13_1C12, 13, {
                {0, 0, 480, 270},
                {480, 0, 480, 270},
                {960, 0, 480, 270},
                {1440, 0, 480, 270},
                {0, 270, 480, 270},
                {480, 270, 960, 540},
                {1440, 270, 480, 270},
                {0, 540, 480, 270},
                {1440, 540, 480, 270},
                {0, 810, 480, 270},
                {480, 810, 480, 270},
                {960, 810, 480, 270},
                {1440, 810, 480, 270}
              }
    },
    {"layout_13_2_4_4_3", LAYOUT_13_2_4_4_3, 13, {
                {480, 0, 480, 270},
                {960, 0, 480, 270},
                {0, 270, 480, 270},
                {480, 270, 480, 270},
                {960, 270, 480, 270},
                {1440, 270, 480, 270},
                {0, 540, 480, 270},
                {480, 540, 480, 270},
                {960, 540, 480, 270},
                {1440, 540, 480, 270},
                {240, 810, 480, 270},
                {720, 810, 480, 270},
                {1200, 810, 480, 270}
              }
    },
    {"layout_14_3_4_4_3", LAYOUT_14_3_4_4_3, 14, {
                {135, 0, 480, 270},
                {615, 0, 480, 270},
                {1095, 0, 480, 270},
                {0, 270, 480, 270},
                {480, 270, 480, 270},
                {960, 270, 480, 270},
                {1440, 270, 480, 270},
                {0, 540, 480, 270},
                {480, 540, 480, 270},
                {960, 540, 480, 270},
                {1440, 540, 480, 270},
                {240, 810, 480, 270},
                {720, 810, 480, 270},
                {1200, 810, 480, 270}
              }
    },
    {"layout_14_2V12", LAYOUT_14_2V12, 14, {
                {0, 0, 960, 720},
                {960, 0, 960, 720},
                {0, 720, 320, 180},
                {320, 720, 320, 180},
                {640, 720, 320, 180},
                {960, 720, 320, 180},
                {1280, 720, 320, 180},
                {1600, 720, 320, 180},
                {0, 900, 320, 180},
                {320, 900, 320, 180},
                {640, 900, 320, 180},
                {960, 900, 320, 180},
                {1280, 900, 320, 180},
                {1600, 900, 320, 180}
              }
    },
    {"layout_15_4_4_4_3", LAYOUT_15_4_4_4_3, 15, {
                {0, 0, 480, 270},
                {480, 0, 480, 270},
                {960, 0, 480, 270},
                {1440, 0, 480, 270},
                {0, 270, 480, 270},
                {480, 270, 480, 270},
                {960, 270, 480, 270},
                {1440, 270, 480, 270},
                {0, 540, 480, 270},
                {480, 540, 480, 270},
                {960, 540, 480, 270},
                {1440, 540, 480, 270},
                {240, 810, 480, 270},
                {720, 810, 480, 270},
                {1200, 810, 480, 270}
              }
    },
    {"layout_16_4x4", LAYOUT_16_4X4, 16, {
                {0, 0, 480, 270},
                {480, 0, 480, 270},
                {960, 0, 480, 270},
                {1440, 0, 480, 270},
                {0, 270, 480, 270},
                {480, 270, 480, 270},
                {960, 270, 480, 270},
                {1440, 270, 480, 270},
                {0, 540, 480, 270},
                {480, 540, 480, 270},
                {960, 540, 480, 270},
                {1440, 540, 480, 270},
                {0, 810, 480, 270},
                {480, 810, 480, 270},
                {960, 810, 480, 270},
                {1440, 810, 480, 270}
              }
    }
};

void MixVideoFrame(unsigned char *frame, const int width, const int height, const int posx, const int posy);
void DrawBound(unsigned char *frame, const int width, const int height, const int posx, const int posy);
struct VideoMixPosition calc_videomixpn[]={
    {"layout_01_1x1" , LAYOUT_01_1X1, 1,{} },
    {"layout_02_2x1", LAYOUT_02_2X1, 2, {} },
    {"layout_03_1_2", LAYOUT_03_1_2, 3, {} },
    {"layout_04_2x2", LAYOUT_04_2X2, 4, {} },
    {"layout_05_2P3", LAYOUT_05_2P3, 5, {} },
    {"layout_06_1P5", LAYOUT_06_1P5, 6, {} },
    {"layout_06_3x2", LAYOUT_06_3X2, 6, {} },
    {"layout_07_2_3_2", LAYOUT_07_2_3_2, 7, {} },
    {"layout_08_1P7", LAYOUT_08_1P7, 8, {} },
    {"layout_08_3_3_2", LAYOUT_08_3_3_2, 8, {} },
    {"layout_09_3x3", LAYOUT_09_3X3, 9, {} },
    {"layout_10_1P9", LAYOUT_10_1P9, 10, {} },
    {"layout_10_2P8", LAYOUT_10_2P8, 10, {} },
    {"layout_10_2V8", LAYOUT_10_2V8, 10, {} },
    {"layout_10_3_4_3", LAYOUT_10_3_4_3, 10, {} },
    {"layout_11_1P10", LAYOUT_11_1P10, 11, {} },
    {"layout_11_4_3_4", LAYOUT_11_4_3_4, 11, {} },
    {"layout_12_4x3", LAYOUT_12_4X3, 12, {} },
    {"layout_12_2P10", LAYOUT_12_2P10, 12, {} },
    {"layout_12_2V10", LAYOUT_12_2V10, 12, {} },
    {"layout_12_1P11", LAYOUT_12_1P11, 12, {} },
    {"layout_13_1P12", LAYOUT_13_1P12, 13, {} },
    {"layout_13_1C12", LAYOUT_13_1C12, 13, {} },
    {"layout_13_2_4_4_3", LAYOUT_13_2_4_4_3, 13, {} },
    {"layout_14_3_4_4_3", LAYOUT_14_3_4_4_3, 14, {} },
    {"layout_14_2V12", LAYOUT_14_2V12, 14, {} },
    {"layout_15_4_4_4_3", LAYOUT_15_4_4_4_3, 15, {} },
    {"layout_16_4x4", LAYOUT_16_4X4, 16, {} }
};

int FindLayoutIndexByLayoutName(const char* layoutName)
{
    int layout_size = sizeof(calc_videomixpn) / sizeof(calc_videomixpn[0]);
    int i = 0;
    for(i = 0; i < layout_size; i++)
    {
        if(!strcmp(calc_videomixpn[i].layout_name,layoutName))
        {
            return i;
        }
    }
    return -1;
}
void set_position4layout_01_1x1(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_01_1x1");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    vmp->pn[0].width = width;
    vmp->pn[0].height = height;
    vmp->pn[0].posx = 0;
    vmp->pn[0].posy = 0;
}
void set_position4layout_02_2x1(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_02_2x1");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    for(i = 0; i < vmp->num; i++)
    {
        vmp->pn[i].width = width >> 1;
        vmp->pn[i].height = height >> 1;
        vmp->pn[i].posx = (width >> 1) * i;
        vmp->pn[i].posy = ((height >> 1) >> 1);
    }
}
void set_position4layout_03_1_2(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_03_1_2");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    for(i = 0; i < vmp->num; i++)
    {
        vmp->pn[i].width = width >> 1;
        vmp->pn[i].height = height >> 1;
        switch(i)
        {
            case 0:
                vmp->pn[i].posx = (width >> 1) >> 1;
                vmp->pn[i].posy = 0;
                break;
            case 1:
            case 2:
                vmp->pn[i].posx = (width >> 1) * (i - 1);
                vmp->pn[i].posy = height >> 1;
                break;
        }
    }
}
void set_position4layout_04_2x2(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_04_2x2");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    for(i = 0; i < vmp->num; i++)
    {
        vmp->pn[i].width = width >> 1;
        vmp->pn[i].height = height >> 1;
        vmp->pn[i].posx = (width >> 1) * (i%2);
        vmp->pn[i].posy = (height >> 1) * (i/2);
    }
}
void set_position4layout_05_2P3(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_05_2P3");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int _width_2_0 = width >> 1;
    int _height_2_0 = _width_2_0 * height / width;;
    int _width_3_0 = width / 3;
    int _height_3_0 = _width_3_0 * height / width;;
    int pad_y = (height - (_height_2_0 + _height_3_0)) >> 1;
    for(i = 0; i < 2; i++)
    {
        vmp->pn[i].width = _width_2_0;
        vmp->pn[i].height = _height_2_0;
        vmp->pn[i].posx = _width_2_0 * (i%2);
        vmp->pn[i].posy = pad_y;
    }
    for(i = 2; i < vmp->num; i++)
    {
        vmp->pn[i].width = _width_3_0;
        vmp->pn[i].height = _height_3_0;
        vmp->pn[i].posx = _width_3_0 * ((i+1)%3);
        vmp->pn[i].posy = pad_y + _height_2_0;
    }
}

void set_position4layout_06_1P5(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_06_1P5");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int _width = width / 3;
    int _height = height / 3;
    for(i = 0; i < vmp->num; i++)
    {
        vmp->pn[i].width = _width;
        vmp->pn[i].height = _height;
        vmp->pn[i].posx = _width * (i%3);
        vmp->pn[i].posy = _height * (i/3);
        if(i == 4)
        {
            vmp->pn[i].width = _width << 1;
            vmp->pn[i].height = _height << 1;
        }
        else if(i == 5)
        {
            vmp->pn[i].posx = 0;
            vmp->pn[i].posy = _height * 2;
        }
    }
}
void set_position4layout_06_3x2(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_06_3x2");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int pad_y = (height / 3) >> 1;
    for(i = 0; i < vmp->num; i++)
    {
        vmp->pn[i].width = width / 3;
        vmp->pn[i].height = height / 3;
        vmp->pn[i].posx = (width / 3) * (i%3);
        vmp->pn[i].posy = (height / 3) * (i/3) + pad_y;
    }
}
void set_position4layout_07_2_3_2(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_07_2_3_2");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int pad_x = (width / 3) >> 1;
    for(i = 0; i < 2; i++)
    {
        vmp->pn[i].width = width / 3;
        vmp->pn[i].height = height / 3;
        vmp->pn[i].posx = (width / 3) * i + pad_x;
        vmp->pn[i].posy = 0;
    }
    for(i = 2; i < vmp->num; i++)
    {
        vmp->pn[i].width = width / 3;
        vmp->pn[i].height = height / 3;
        vmp->pn[i].posx = (width / 3) * ((i+1)%3);
        vmp->pn[i].posy = (height / 3) * ((i+1)/3);
        if((i+1)/3 == 2)//line 3
        {
            vmp->pn[i].posx = (width / 3) * ((i+1)%3) + pad_x;
        }
    }
}
void set_position4layout_08_1P7(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_08_1P7");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int _width = width >> 2;
    int _height = height >> 2;
    for(i=0; i < vmp->num; i++)
    {
        vmp->pn[i].width = _width;
        vmp->pn[i].height = _height;
        vmp->pn[i].posx = _width * (i%4);
        vmp->pn[i].posy = _height * (i/4);
        if(i == 5)
        {
            vmp->pn[i].width = _width * 3;
            vmp->pn[i].height = _height * 3;
            vmp->pn[i].posx = _width * (i%4);
            vmp->pn[i].posy = _height * (i/4);
        }
        if(i > 5)
        {
            vmp->pn[i].posx = 0;
            vmp->pn[i].posy = _height * (i-4);
        }
    }
    
}
void set_position4layout_08_3_3_2(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_08_3_3_2");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int pad_x = (width / 3) >> 1;
    for(i = 0; i < vmp->num; i++)
    {
        vmp->pn[i].width = width / 3;
        vmp->pn[i].height = height / 3;
        vmp->pn[i].posx = (width / 3) * (i%3);
        vmp->pn[i].posy = (height / 3) * (i/3);
        if(i/3 == 2)//line 3
        {
            vmp->pn[i].posx = (width / 3) * (i%3) + pad_x;
        }
    }
}
void set_position4layout_09_3x3(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_09_3x3");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    for(i = 0; i < vmp->num; i++)
    {
        vmp->pn[i].width = width / 3;
        vmp->pn[i].height = height / 3;
        vmp->pn[i].posx = (width / 3) * (i%3);
        vmp->pn[i].posy = (height / 3) * (i/3);
    }
}
void set_position4layout_10_1P9(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_10_1P9");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int _width = width / 5;
    int _height = height / 5;
    for(i=0; i < vmp->num; i++)
    {
        vmp->pn[i].width = _width;
        vmp->pn[i].height = _height;
        vmp->pn[i].posx = _width * (i%5);
        vmp->pn[i].posy = _height * (i/5);
        if(i == 6)
        {
            vmp->pn[i].width = _width * 4;
            vmp->pn[i].height = _height * 4;
            vmp->pn[i].posx = _width * (i%5);
            vmp->pn[i].posy = _height * (i/5);
        }
        if(i > 6)
        {
            vmp->pn[i].posx = 0;
            vmp->pn[i].posy = _height * (i-5);
        }
    }
}
void set_position4layout_10_2P8(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_10_2P8");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int _width = width >> 2;
    int _height = height >> 2;
    for(i=0; i < 4; i++)
    {
        vmp->pn[i].width = _width;
        vmp->pn[i].height = _height;
        vmp->pn[i].posx = _width * (i%4);
        vmp->pn[i].posy = _height * (i/4);
    }
    for(i=4; i < 6; i++)
    {
        vmp->pn[i].width = _width << 1;
        vmp->pn[i].height = _height << 1;
        vmp->pn[i].posx = vmp->pn[i].width * (i%4);
        vmp->pn[i].posy = _height * (i/4);
    }
    for(i=6; i < vmp->num; i++)
    {
        vmp->pn[i].width = _width;
        vmp->pn[i].height = _height;
        vmp->pn[i].posx = _width * ((i+2)%4);
        vmp->pn[i].posy = _height * ((i+2)/4 + 1);
    }
}
void set_position4layout_10_2V8(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_10_2V8");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int _width = width >> 2;
    int _height = height >> 2;
    for(i=0; i < 2; i++)
    {
        vmp->pn[i].width = _width << 1;
        vmp->pn[i].height = _height << 1;
        vmp->pn[i].posx = vmp->pn[i].width * (i%2);
        vmp->pn[i].posy = vmp->pn[i].height * (i/2);
    }
    for(i=2; i < vmp->num; i++)
    {
        vmp->pn[i].width = _width;
        vmp->pn[i].height = _height;
        vmp->pn[i].posx = _width * ((i+2)%4);
        vmp->pn[i].posy = _height * ((i+2)/4 + 1);
    }
}
void set_position4layout_10_3_4_3(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_10_3_4_3");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int _width = width >> 2;
    int _height = height >> 2;
    int pad_x = _width >> 1;
    int pad_y = _height >> 1;
    for(i=0; i < vmp->num; i++)
    {
        vmp->pn[i].width = _width;
        vmp->pn[i].height = _height;
        vmp->pn[i].posx = _width * (i%4) + pad_x;
        vmp->pn[i].posy = _height * (i/4) + pad_y;
        if(i >= 3 && i < 7)
        {
            vmp->pn[i].posx = _width * ((i+1)%4);
            vmp->pn[i].posy = _height * ((i+1)/4) + pad_y;
        }
        if(i >= 7)
        {
            vmp->pn[i].posx = _width * ((i+1)%4) + pad_x;
            vmp->pn[i].posy = _height * ((i+1)/4) + pad_y;
        }
    }
}
void set_position4layout_11_1P10(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_11_1P10");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int _width = width >> 2;
    int _height = height / 3;
    for(i=0; i < vmp->num; i++)
    {
        vmp->pn[i].width = _width;
        vmp->pn[i].height = _height;
        vmp->pn[i].posx = _width * (i%4);
        vmp->pn[i].posy = _height * (i/4);
        if(i == 5)
        {
            vmp->pn[i].width = _width << 1;
            vmp->pn[i].height = _height;
        }
        if(i == 6)
        {
            vmp->pn[i].posx = _width * (i%4 + 1);
            vmp->pn[i].posy = _height * (i/4);
        }
        if(i > 6)
        {
            vmp->pn[i].posx = _width * ((i+1)%4);
            vmp->pn[i].posy = _height * ((i+1)/4);
        }
    }
}
void set_position4layout_11_4_3_4(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_11_4_3_4");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int _width = width >> 2;
    int _height = height >> 2;
    int pad_x = _width >> 1;
    int pad_y = _height >> 1;
    for(i = 0; i < vmp->num; i++)
    {
        vmp->pn[i].width = _width;
        vmp->pn[i].height = _height;
        vmp->pn[i].posx = _width * (i%4);
        vmp->pn[i].posy = _height * (i/4) + pad_y;
        if(i > 3)
        {
            vmp->pn[i].posx = _width * (i%4) + pad_x;
        }
        if(i > 6)
        {
            vmp->pn[i].posx = _width * ((i+1)%4);
            vmp->pn[i].posy = _height * ((i+1)/4) + pad_y;
        }
    }
}
void set_position4layout_12_4x3(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_12_4x3");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int pad_y = height >> 2 >> 1;
    for(i = 0; i < vmp->num; i++)
    {
        vmp->pn[i].width = width >> 2;
        vmp->pn[i].height = height >> 2;
        vmp->pn[i].posx = (width >> 2) * (i%4);
        vmp->pn[i].posy = (height >> 2) * (i/4) + pad_y;
    }
}
void set_position4layout_12_2P10(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_12_2P10");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int _width = width / 5;
    int _height = height / 5;
    for(i = 0; i < vmp->num; i++)
    {
        vmp->pn[i].width = _width;
        vmp->pn[i].height = _height;
        vmp->pn[i].posx = _width * (i%5);
        vmp->pn[i].posy = _height * (i/5);
        if(i > 4 && i <= 6)
        {
            vmp->pn[i].width = (_width * 5) >> 1;
            vmp->pn[i].height = _height * 3;
            vmp->pn[i].posx = vmp->pn[i].width * (i%5);
            vmp->pn[i].posy = _height * (i/5);
        }
        if(i > 6)
        {
            vmp->pn[i].posx = _width * ((i+3)%5);
            vmp->pn[i].posy = _height * ((i+3)/5 + 2);
        }
    }
}
void set_position4layout_12_2V10(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_12_2V10");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int _width = width / 5;
    int _height = height / 5;
    for(i = 0; i < vmp->num; i++)
    {
        vmp->pn[i].width = _width;
        vmp->pn[i].height = _height;
        if(i < 2)
        {
            vmp->pn[i].width = (_width * 5) >> 1;
            vmp->pn[i].height = _height * 3;
            vmp->pn[i].posx = vmp->pn[i].width * (i%5);
            vmp->pn[i].posy = _height * (i/5);
        }
        else
        {
            vmp->pn[i].posx = _width * ((i+3)%5);
            vmp->pn[i].posy = _height * ((i+3)/5 + 2);
        }
    }
}
void set_position4layout_12_1P11(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_12_1P11");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int _width = width / 6;
    int _height = height / 6;
    for(i = 0; i < vmp->num; i++)
    {
        vmp->pn[i].width = _width;
        vmp->pn[i].height = _height;
        vmp->pn[i].posx = _width * (i%6);
        vmp->pn[i].posy = _height * (i/6);
        if(i == 7)
        {
            vmp->pn[i].width = _width * 5;
            vmp->pn[i].height = _height * 5;
        }
        if(i > 7)
        {
            vmp->pn[i].posx = 0;
            vmp->pn[i].posy = _height * (i - 6);
        }
    }
}
void set_position4layout_13_1P12(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_13_1P12");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int _width = width >> 2;
    int _height = height >> 2;

    vmp->pn[0].width = _width << 1;
    vmp->pn[0].height = _height << 1;
    vmp->pn[0].posx = 0;
    vmp->pn[0].posy = 0;

    for(i = 1; i < vmp->num; i++)
    {
        vmp->pn[i].width = _width;
        vmp->pn[i].height = _height;
        if(i <= 2)
        {
            vmp->pn[i].posx = _width * ((i+1)%4);
            vmp->pn[i].posy = _height *((i+1)/4);
        }
        else if(i > 2)
        {
            vmp->pn[i].posx = _width * ((i+3)%4);
            vmp->pn[i].posy = _height *((i+3)/4);
        }
    }
}
void set_position4layout_13_1C12(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_13_1C12");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int _width = width >> 2;
    int _height = height >> 2;
    for(i = 0; i < vmp->num; i++)
    {
        vmp->pn[i].width = _width;
        vmp->pn[i].height = _height;
        vmp->pn[i].posx = _width * (i%4);
        vmp->pn[i].posy = _height *(i/4);
        if(i == 5)
        {
            vmp->pn[i].width = _width << 1;
            vmp->pn[i].height = _height << 1;
        }
        else if(i > 5 && i <= 7)
        {
            vmp->pn[i].posx = _width * ((i+1)%4);
            vmp->pn[i].posy = _height *((i+1)/4);
        }
        else if(i > 7)
        {
            vmp->pn[i].posx = _width * ((i+3)%4);
            vmp->pn[i].posy = _height *((i+3)/4);
        }
    }
}
void set_position4layout_13_2_4_4_3(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_13_2_4_4_3");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int pad_x = width >> 2;
    for(i = 0; i < 2; i++)
    {
        vmp->pn[i].width = width >> 2;
        vmp->pn[i].height = height >> 2;
        vmp->pn[i].posx = (width >> 2) * (i%4) + pad_x;
        vmp->pn[i].posy = (height >> 2) * (i/4);
    }
    for(i = 2; i < vmp->num; i++)
    {
        vmp->pn[i].width = width >> 2;
        vmp->pn[i].height = height >> 2;
        vmp->pn[i].posx = (width >> 2) * ((i+2)%4);
        vmp->pn[i].posy = (height >> 2) * ((i+2)/4);
        if((i+2)/4 == 3)//line 4
        {
            vmp->pn[i].posx = (width >> 2) * ((i+2)%4) + (pad_x >> 1);
        }
    }
}
void set_position4layout_14_3_4_4_3(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_14_3_4_4_3");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int pad_x = width >> 2 >> 1;
    for(i = 0; i < 3; i++)
    {
        vmp->pn[i].width = width >> 2;
        vmp->pn[i].height = height >> 2;
        vmp->pn[i].posx = (width >> 2) * (i%4) + pad_x;
        vmp->pn[i].posy = (height >> 2) * (i/4);
    }
    for(i = 3; i < vmp->num; i++)
    {
        vmp->pn[i].width = width >> 2;
        vmp->pn[i].height = height >> 2;
        vmp->pn[i].posx = (width >> 2) * ((i+1)%4);
        vmp->pn[i].posy = (height >> 2) * ((i+1)/4);
        if((i+1)/4 == 3)//line 4
        {
            vmp->pn[i].posx = (width >> 2) * ((i+1)%4) + pad_x;
        }
    }
}
void set_position4layout_14_2V12(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_14_2V12");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int _width = width / 6;
    int _height = height / 6;
    for(i = 0;i < 2;i++)
    {
        vmp->pn[i].width = _width * 3;
        vmp->pn[i].height = _height * 4;
        vmp->pn[i].posx = vmp->pn[i].width * i;
        vmp->pn[i].posy = 0;
    }

    for(i = 2;i < vmp->num;i++)
    {
        vmp->pn[i].width = _width;
        vmp->pn[i].height = _height;
        vmp->pn[i].posx = ((i-2)%6)*_width;
        vmp->pn[i].posy = ((i-2)/6)*_height + vmp->pn[0].height;
    }
}
void set_position4layout_15_4_4_4_3(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_15_4_4_4_3");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    int pad_x = width >> 2 >> 1;
    for(i = 0; i < vmp->num; i++)
    {
        vmp->pn[i].width = width >> 2;
        vmp->pn[i].height = height >> 2;
        vmp->pn[i].posx = (width >> 2) * (i%4);
        vmp->pn[i].posy = (height >> 2) * (i/4);
        if((i/4) == 3)//line 4
        {
            vmp->pn[i].posx = (width >> 2) * (i%4) + pad_x;
        }
    }
}
void set_position4layout_16_4x4(const int width, const int height)
{
    int idx = FindLayoutIndexByLayoutName("layout_16_4x4");
    struct VideoMixPosition *vmp = &calc_videomixpn[idx];
    int i = 0;
    for(i = 0; i < vmp->num; i++)
    {
        vmp->pn[i].width = width >> 2;
        vmp->pn[i].height = height >> 2;
        vmp->pn[i].posx = (width >> 2) * (i%4);
        vmp->pn[i].posy = (height >> 2) * (i/4);
    }
}
int main(int argc, char **argv)
{
    int layout_idx = -1;
    layout_idx = atoi(argv[1]);
    int frame_size = WIDTH * HEIGHT * 3 >> 1;
    unsigned char *frame = (unsigned char*)malloc(frame_size);
    /*
    gray:125
    green:0
    pink:255
    */
    set_position4layout_01_1x1(WIDTH, HEIGHT);
    set_position4layout_02_2x1(WIDTH, HEIGHT);
    set_position4layout_03_1_2(WIDTH, HEIGHT);
    set_position4layout_04_2x2(WIDTH, HEIGHT);
    set_position4layout_09_3x3(WIDTH, HEIGHT);
    set_position4layout_16_4x4(WIDTH, HEIGHT);
    set_position4layout_05_2P3(WIDTH, HEIGHT);
    set_position4layout_06_1P5(WIDTH, HEIGHT);
    set_position4layout_06_3x2(WIDTH, HEIGHT);
    set_position4layout_07_2_3_2(WIDTH, HEIGHT);
    set_position4layout_08_1P7(WIDTH, HEIGHT);
    set_position4layout_08_3_3_2(WIDTH, HEIGHT);
    set_position4layout_10_1P9(WIDTH, HEIGHT);
    set_position4layout_10_2P8(WIDTH, HEIGHT);
    set_position4layout_10_2V8(WIDTH, HEIGHT);
    set_position4layout_10_3_4_3(WIDTH, HEIGHT);
    set_position4layout_11_1P10(WIDTH, HEIGHT);
    set_position4layout_11_4_3_4(WIDTH, HEIGHT);
    set_position4layout_12_4x3(WIDTH, HEIGHT);
    set_position4layout_12_2P10(WIDTH, HEIGHT);
    set_position4layout_12_2V10(WIDTH, HEIGHT);
    set_position4layout_12_1P11(WIDTH, HEIGHT);
    set_position4layout_13_1P12(WIDTH, HEIGHT);
    set_position4layout_13_1C12(WIDTH, HEIGHT);
    set_position4layout_13_2_4_4_3(WIDTH, HEIGHT);
    set_position4layout_14_3_4_4_3(WIDTH, HEIGHT);
    set_position4layout_14_2V12(WIDTH, HEIGHT);
    set_position4layout_15_4_4_4_3(WIDTH, HEIGHT);
    memset(frame, 255, frame_size);
    char filename[64] = {0};
    FILE *fp = NULL;
    int len = sizeof(calc_videomixpn) / sizeof(calc_videomixpn[0]);
    for(int i = layout_idx; i < layout_idx + 1; i++)
    {
        sprintf(filename,"/tmp/%s.yuv",calc_videomixpn[i].layout_name);
        fp = fopen(filename,"wb");
        cout << "name=" << calc_videomixpn[i].layout_name<< endl;
        cout << "filename=" << filename<< endl;
        for(int j = 0; j < calc_videomixpn[i].num; j++)
        {
            cout << "layout name:" << calc_videomixpn[i].layout_name << ", num= "  << j <<endl;
            cout << "  posx=" << calc_videomixpn[i].pn[j].posx << endl;
            cout << "  posy=" << calc_videomixpn[i].pn[j].posy << endl;
            cout << "  width=" << calc_videomixpn[i].pn[j].width<< endl;
            cout << "  height=" << calc_videomixpn[i].pn[j].height<< endl;
            DrawBound(frame, calc_videomixpn[i].pn[j].width, calc_videomixpn[i].pn[j].height, calc_videomixpn[i].pn[j].posx, calc_videomixpn[i].pn[j].posy);
            //MixVideoFrame(frame, calc_videomixpn[i].pn[j].width, calc_videomixpn[i].pn[j].height, calc_videomixpn[i].pn[j].posx, calc_videomixpn[i].pn[j].posy);
        }
    }
    fwrite(frame,1,frame_size,fp);
    fclose(fp);
    free(frame);
}
void DrawBound(unsigned char *frame, const int width, const int height, const int posx, const int posy)
{
    int i = 0,j = 0;
    unsigned char *y_buf = frame;
    unsigned char *u_buf = frame + WIDTH * HEIGHT;
    unsigned char *v_buf = u_buf + WIDTH * HEIGHT / 4;
    for( j = 0; j < height; j++)
    {
        if(j ==0 || j == (height - 1) )
        {
            memset(y_buf + (posy + j)* WIDTH + posx, 125, width);
        }
        else
        {
            y_buf[(posy + j)*WIDTH + posx] = 125;
            y_buf[(posy + j)*WIDTH + posx + width -1] = 125;
        }
        if(j < height >> 1)
        {
            if(j == 0 || (j == ((height >> 1) - 1)))
            {
                memset(u_buf + (posy/2 + j)*WIDTH/2+ posx/2, 125, width/2);
                memset(v_buf + (posy/2 + j)*WIDTH/2+ posx/2, 125, width/2);
            }
            else
            {
                u_buf[(posy/2 + j)*WIDTH/2 + posx/2] = 125;
                u_buf[(posy/2 + j)*WIDTH/2 + posx/2 + width/2 -1] = 125;
                v_buf[(posy/2 + j)*WIDTH/2 + posx/2] = 125;
                v_buf[(posy/2 + j)*WIDTH/2 + posx/2 + width/2 -1] = 125;
            }
        }
    }
}

void MixVideoFrame(unsigned char *frame, const int width, const int height, const int posx, const int posy)
{
    int i = 0,j = 0;
    unsigned char *y_buf = frame;
    unsigned char *u_buf = frame + WIDTH * HEIGHT;
    unsigned char *v_buf = u_buf + WIDTH * HEIGHT / 4;
    for( j = 0; j < height; j++)
    {
        memset(y_buf + (posy + j)* WIDTH + posx, 125, width);
        if(j < height >> 1)
        {
            memset(u_buf + (posy/2 + j)*WIDTH/2+ posx/2, 125, width/2);
            memset(v_buf + (posy/2 + j)*WIDTH/2+ posx/2, 125, width/2);
        }
    }
}

//g++ layout.cxx -o layout
