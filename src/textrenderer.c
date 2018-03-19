#include "textrenderer.h"
#include "bitmap.h"
#include <math.h>
#include <stdio.h>
#include <windows.h>


int lut[256][9];


void trCreateFont() {

    for(int i=0; i<256; i++) {
        memcpy(lut[i], (int[]) {0,
                                 8010,
                                 8101,
                                 8010,
                                 8101,
                                 8010,
                                 8101, 8000, 8000}, sizeof(lut[i]));
    }


    // space
    memcpy(lut[32], (int[]) {0,
                             8000,
                             8000,
                             8000,
                             8000,
                             8000,
                             8000, 8000, 8000}, sizeof(lut[32]));

    // A
    memcpy(lut[65], (int[]) {0,
                             8010,
                             8101,
                             8101,
                             8111,
                             8101,
                             8101, 8000, 8000}, sizeof(lut[65]));

    // B
    memcpy(lut[66], (int[]) {0,
                             8110,
                             8101,
                             8110,
                             8101,
                             8101,
                             8110, 8000, 8000}, sizeof(lut[66]));

    // C
    memcpy(lut[67], (int[]) {0,
                             8011,
                             8100,
                             8100,
                             8100,
                             8100,
                             8011, 8000, 8000}, sizeof(lut[67]));

    // D
    memcpy(lut[68], (int[]) {0,
                             8110,
                             8101,
                             8101,
                             8101,
                             8101,
                             8110, 8000, 8000}, sizeof(lut[68]));

    // E
    memcpy(lut[69], (int[]) {0,
                             8111,
                             8100,
                             8110,
                             8100,
                             8100,
                             8111, 8000, 8000}, sizeof(lut[69]));

    // F
    memcpy(lut[70], (int[]) {0,
                             8111,
                             8100,
                             8110,
                             8100,
                             8100,
                             8100, 8000, 8000}, sizeof(lut[70]));

    // G
    memcpy(lut[71], (int[]) {0,
                             8011,
                             8100,
                             8100,
                             8101,
                             8101,
                             8011, 8000, 8000}, sizeof(lut[71]));

    // H
    memcpy(lut[72], (int[]) {0,
                             8101,
                             8101,
                             8111,
                             8101,
                             8101,
                             8101, 8000, 8000}, sizeof(lut[72]));

    // I
    memcpy(lut[73], (int[]) {0,
                             8111,
                             8010,
                             8010,
                             8010,
                             8010,
                             8111, 8000, 8000}, sizeof(lut[73]));

    // J
    memcpy(lut[74], (int[]) {0,
                             8001,
                             8001,
                             8001,
                             8001,
                             8001,
                             8110, 8000, 8000}, sizeof(lut[74]));

    // K
    memcpy(lut[75], (int[]) {0,
                             8101,
                             8101,
                             8110,
                             8110,
                             8101,
                             8101, 8000, 8000}, sizeof(lut[75]));

    // L
    memcpy(lut[76], (int[]) {0,
                             8100,
                             8100,
                             8100,
                             8100,
                             8100,
                             8111, 8000, 8000}, sizeof(lut[76]));

    // M
    memcpy(lut[77], (int[]) {0,
                             8101,
                             8111,
                             8111,
                             8101,
                             8101,
                             8101, 8000, 8000}, sizeof(lut[77]));

    // N
    memcpy(lut[78], (int[]) {0,
                             8101,
                             8101,
                             8111,
                             8111,
                             8101,
                             8101, 8000, 8000}, sizeof(lut[78]));

    // O
    memcpy(lut[79], (int[]) {0,
                             8111,
                             8101,
                             8101,
                             8101,
                             8101,
                             8111, 8000, 8000}, sizeof(lut[79]));

    // P
    memcpy(lut[80], (int[]) {0,
                             8110,
                             8101,
                             8101,
                             8110,
                             8100,
                             8100, 8000, 8000}, sizeof(lut[80]));

    // Q
    memcpy(lut[81], (int[]) {0,
                             8010,
                             8101,
                             8101,
                             8101,
                             8101,
                             8010,
                             8001, 8000}, sizeof(lut[81]));

    // R
    memcpy(lut[82], (int[]) {0,
                             8110,
                             8101,
                             8101,
                             8110,
                             8101,
                             8101, 8000, 8000}, sizeof(lut[82]));

    // S
    memcpy(lut[83], (int[]) {0,
                             8011,
                             8100,
                             8010,
                             8001,
                             8001,
                             8110, 8000, 8000}, sizeof(lut[83]));

    // T
    memcpy(lut[84], (int[]) {0,
                             8111,
                             8010,
                             8010,
                             8010,
                             8010,
                             8010, 8000, 8000}, sizeof(lut[84]));

    // U
    memcpy(lut[85], (int[]) {0,
                             8101,
                             8101,
                             8101,
                             8101,
                             101,
                             8011, 8000, 8000}, sizeof(lut[85]));

    // V
    memcpy(lut[86], (int[]) {0,
                             8101,
                             8101,
                             8101,
                             8101,
                             8010,
                             8010, 8000, 8000}, sizeof(lut[86]));

    // W
    memcpy(lut[87], (int[]) {0,
                             8101,
                             8101,
                             8101,
                             8111,
                             8111,
                             8101, 8000, 8000}, sizeof(lut[87]));

    // X
    memcpy(lut[88], (int[]) {0,
                             8101,
                             8101,
                             8010,
                             8010,
                             8101,
                             8101, 8000, 8000}, sizeof(lut[88]));

    // Y
    memcpy(lut[89], (int[]) {0,
                             8101,
                             8101,
                             8010,
                             8010,
                             8010,
                             8010, 8000, 8000}, sizeof(lut[89]));

    // Z
    memcpy(lut[90], (int[]) {0,
                             8111,
                             8001,
                             8010,
                             8100,
                             8100,
                             8111, 8000, 8000}, sizeof(lut[90]));


    // a
    memcpy(lut[97], (int[]) {0,
                             8000,
                             8000,
                             8011,
                             8101,
                             8101,
                             8011, 8000, 8000}, sizeof(lut[97]));

    // b
    memcpy(lut[98], (int[]) {0,
                             8100,
                             8100,
                             8110,
                             8101,
                             8101,
                             8110, 8000, 8000}, sizeof(lut[98]));

    // c
    memcpy(lut[99], (int[]) {0,
                             8000,
                             8000,
                             8011,
                             8100,
                             8100,
                             8011, 8000, 8000}, sizeof(lut[99]));

    // d
    memcpy(lut[100], (int[]) {0,
                             8001,
                             8001,
                             8011,
                             8101,
                             8101,
                             8011, 8000, 8000}, sizeof(lut[100]));

    // e
    memcpy(lut[101], (int[]) {0,
                             8000,
                             8000,
                             8011,
                             8111,
                             8100,
                             8011, 8000, 8000}, sizeof(lut[101]));

    // f
    memcpy(lut[102], (int[]) {0,
                             8001,
                             8010,
                             8111,
                             8010,
                             8010,
                             8010, 8000, 8000}, sizeof(lut[102]));

    // g
    memcpy(lut[103], (int[]) {0,
                             8000,
                             8000,
                             8011,
                             8101,
                             8101,
                             8011,
                             8001,
                             8110}, sizeof(lut[103]));

    // h
    memcpy(lut[104], (int[]) {0,
                             8100,
                             8100,
                             8110,
                             8101,
                             8101,
                             8101, 8000, 8000}, sizeof(lut[104]));

    // i
    memcpy(lut[105], (int[]) {0,
                             8010,
                             8000,
                             8010,
                             8010,
                             8010,
                             8010, 8000, 8000}, sizeof(lut[105]));

    // j
    memcpy(lut[106], (int[]) {0,
                             8010,
                             8000,
                             8010,
                             8010,
                             8010,
                             8010,
                             8010,
                             8100}, sizeof(lut[106]));

    // k
    memcpy(lut[107], (int[]) {0,
                             8100,
                             8100,
                             8101,
                             8110,
                             8110,
                             8101, 8000, 8000}, sizeof(lut[107]));

    // l
    memcpy(lut[108], (int[]) {0,
                             8010,
                             8010,
                             8010,
                             8010,
                             8010,
                             8010, 8000, 8000}, sizeof(lut[108]));

    // m
    memcpy(lut[109], (int[]) {0,
                             8000,
                             8000,
                             8111,
                             8111,
                             8101,
                             8101, 8000, 8000}, sizeof(lut[109]));

    // n
    memcpy(lut[110], (int[]) {0,
                             8000,
                             8000,
                             8110,
                             8101,
                             8101,
                             8101, 8000, 8000}, sizeof(lut[110]));

    // o
    memcpy(lut[111], (int[]) {0,
                             8000,
                             8000,
                             8010,
                             8101,
                             8101,
                             8010, 8000, 8000}, sizeof(lut[111]));

    // p
    memcpy(lut[112], (int[]) {0,
                             8000,
                             8000,
                             8110,
                             8101,
                             8101,
                             8110,
                             8100,
                             8100}, sizeof(lut[112]));

    // q
    memcpy(lut[113], (int[]) {0,
                             8000,
                             8000,
                             8011,
                             8101,
                             8101,
                             8011,
                             8001,
                             8001}, sizeof(lut[113]));

    // r
    memcpy(lut[114], (int[]) {0,
                             8000,
                             8000,
                             8111,
                             8100,
                             8100,
                             8100, 8000, 8000}, sizeof(lut[114]));

    // s
    memcpy(lut[115], (int[]) {0,
                             8000,
                             8000,
                             8011,
                             8110,
                             8001,
                             8110, 8000, 8000}, sizeof(lut[115]));

    // t
    memcpy(lut[116], (int[]) {0,
                             8000,
                             8100,
                             8111,
                             8100,
                             8100,
                             8011, 8000, 8000}, sizeof(lut[116]));

    // u
    memcpy(lut[117], (int[]) {0,
                             8000,
                             8000,
                             8101,
                             8101,
                             8101,
                             8011, 8000, 8000}, sizeof(lut[117]));

    // v
    memcpy(lut[118], (int[]) {0,
                             8000,
                             8000,
                             8101,
                             8101,
                             8101,
                             8010, 8000, 8000}, sizeof(lut[118]));

    // w
    memcpy(lut[119], (int[]) {0,
                             8000,
                             8000,
                             8101,
                             8101,
                             8111,
                             8111, 8000, 8000}, sizeof(lut[119]));

    // x
    memcpy(lut[120], (int[]) {0,
                             8000,
                             8000,
                             8101,
                             8010,
                             8010,
                             8101, 8000, 8000}, sizeof(lut[120]));

    // y
    memcpy(lut[121], (int[]) {0,
                             8000,
                             8000,
                             8101,
                             8101,
                             8101,
                             8011,
                             8001,
                             8110}, sizeof(lut[121]));

    // z
    memcpy(lut[122], (int[]) {0,
                             8000,
                             8000,
                             8111,
                             8010,
                             8100,
                             8111, 8000, 8000}, sizeof(lut[122]));

    // 0
    memcpy(lut[48], (int[]) {0,
                             8010,
                             8101,
                             8101,
                             8101,
                             8101,
                             8010, 8000, 8000}, sizeof(lut[48]));

    // 1
    memcpy(lut[49], (int[]) {0,
                             8010,
                             8110,
                             8010,
                             8010,
                             8010,
                             8010, 8000, 8000}, sizeof(lut[49]));

    // 2
    memcpy(lut[50], (int[]) {0,
                             8110,
                             8001,
                             8001,
                             8010,
                             8100,
                             8111, 8000, 8000}, sizeof(lut[50]));

    // 3
    memcpy(lut[51], (int[]) {0,
                             8110,
                             8001,
                             8010,
                             8001,
                             8001,
                             8110, 8000, 8000}, sizeof(lut[51]));

    // 4
    memcpy(lut[52], (int[]) {0,
                             8001,
                             8011,
                             8101,
                             8111,
                             8001,
                             8001, 8000, 8000}, sizeof(lut[52]));

    // 5
    memcpy(lut[53], (int[]) {0,
                             8111,
                             8100,
                             8110,
                             8001,
                             8001,
                             8110, 8000, 8000}, sizeof(lut[53]));

    // 6
    memcpy(lut[54], (int[]) {0,
                             8011,
                             8100,
                             8110,
                             8101,
                             8101,
                             8010, 8000, 8000}, sizeof(lut[54]));

    // 7
    memcpy(lut[55], (int[]) {0,
                             8111,
                             8001,
                             8001,
                             8010,
                             8010,
                             8010, 8000, 8000}, sizeof(lut[55]));

    // 8
    memcpy(lut[56], (int[]) {0,
                             8010,
                             8101,
                             8010,
                             8101,
                             8101,
                             8010, 8000, 8000}, sizeof(lut[56]));

    // 9
    memcpy(lut[57], (int[]) {0,
                             8010,
                             8101,
                             8011,
                             8001,
                             8001,
                             8110, 8000, 8000}, sizeof(lut[57]));


    // !
    memcpy(lut[33], (int[]) {0,
                             8010,
                             8010,
                             8010,
                             8010,
                             8000,
                             8010, 8000, 8000}, sizeof(lut[33]));

    // "
    memcpy(lut[34], (int[]) {0,
                             8101,
                             8101,
                             8000,
                             8000,
                             8000,
                             8000, 8000, 8000}, sizeof(lut[34]));

    // #
    memcpy(lut[35], (int[]) {0,
                             8101,
                             8111,
                             8101,
                             8111,
                             8101,
                             8000, 8000, 8000}, sizeof(lut[35]));

    // $
    memcpy(lut[36], (int[]) {0,
                             8010,
                             8011,
                             8100,
                             8010,
                             8001,
                             8110,
                             8010, 8000}, sizeof(lut[36]));

    // %
    memcpy(lut[37], (int[]) {0,
                             8101,
                             8001,
                             8010,
                             8010,
                             8100,
                             8101, 8000, 8000}, sizeof(lut[37]));

    // &
    memcpy(lut[38], (int[]) {0,
                             8011,
                             8100,
                             8011,
                             8101,
                             8101,
                             8011, 8000, 8000}, sizeof(lut[38]));

    // '
    memcpy(lut[39], (int[]) {0,
                             8010,
                             8010,
                             8000,
                             8000,
                             8000,
                             8000, 8000, 8000}, sizeof(lut[39]));

    // (
    memcpy(lut[40], (int[]) {0,
                             8001,
                             8010,
                             8010,
                             8010,
                             8010,
                             8001, 8000, 8000}, sizeof(lut[40]));

    // )
    memcpy(lut[41], (int[]) {0,
                             8100,
                             8010,
                             8010,
                             8010,
                             8010,
                             8100, 8000, 8000}, sizeof(lut[41]));

    // *
    memcpy(lut[42], (int[]) {0,
                             8101,
                             8010,
                             8111,
                             8010,
                             8101,
                             8000, 8000, 8000}, sizeof(lut[42]));

    // +
    memcpy(lut[43], (int[]) {0,
                             8000,
                             8010,
                             8010,
                             8111,
                             8010,
                             8010, 8000, 8000}, sizeof(lut[43]));

    // ,
    memcpy(lut[44], (int[]) {0,
                             8000,
                             8000,
                             8000,
                             8000,
                             8000,
                             8010,
                             8100, 8000}, sizeof(lut[4]));

    // -
    memcpy(lut[45], (int[]) {0,
                             8000,
                             8000,
                             8000,
                             8111,
                             8000,
                             8000, 8000, 8000}, sizeof(lut[45]));

    // .
    memcpy(lut[46], (int[]) {0,
                             8000,
                             8000,
                             8000,
                             8000,
                             8000,
                             8010, 8000, 8000}, sizeof(lut[46]));

    // /
    memcpy(lut[47], (int[]) {0,
                             8001,
                             8001,
                             8010,
                             8010,
                             8100,
                             8100, 8000, 8000}, sizeof(lut[47]));


    // :
    memcpy(lut[58], (int[]) {0,
                             8000,
                             8010,
                             8000,
                             8000,
                             8010,
                             8000, 8000, 8000}, sizeof(lut[58]));

    // ;
    memcpy(lut[59], (int[]) {0,
                             8000,
                             8000,
                             8000,
                             8010,
                             8000,
                             8010,
                             8100, 8000}, sizeof(lut[59]));

    // <
    memcpy(lut[60], (int[]) {0,
                             8000,
                             8001,
                             8010,
                             8100,
                             8010,
                             8001, 8000, 8000}, sizeof(lut[60]));

    // =
    memcpy(lut[61], (int[]) {0,
                             8000,
                             8000,
                             8111,
                             8000,
                             8111,
                             8000, 8000, 8000}, sizeof(lut[61]));

    // >
    memcpy(lut[62], (int[]) {0,
                             8000,
                             8100,
                             8010,
                             8001,
                             8010,
                             8100, 8000, 8000}, sizeof(lut[62]));

    // ?
    memcpy(lut[63], (int[]) {0,
                             8010,
                             8101,
                             8001,
                             8010,
                             8000,
                             8010, 8000, 8000}, sizeof(lut[63]));

    // @
    memcpy(lut[64], (int[]) {0,
                             8000,
                             8011,
                             8101,
                             8101,
                             8100,
                             8011, 8000, 8000}, sizeof(lut[64]));


    // [
    memcpy(lut[91], (int[]) {0,
                             8011,
                             8010,
                             8010,
                             8010,
                             8010,
                             8011, 8000, 8000}, sizeof(lut[91]));

    // backslash
    memcpy(lut[92], (int[]) {0,
                             8100,
                             8100,
                             8010,
                             8010,
                             8001,
                             8001, 8000, 8000}, sizeof(lut[92]));


    // ]
    memcpy(lut[93], (int[]) {0,
                             8110,
                             8010,
                             8010,
                             8010,
                             8010,
                             8110, 8000, 8000}, sizeof(lut[93]));

    // ^
    memcpy(lut[94], (int[]) {0,
                             8010,
                             8101,
                             8000,
                             8000,
                             8000,
                             8000, 8000, 8000}, sizeof(lut[94]));

    // _
    memcpy(lut[95], (int[]) {0,
                             8000,
                             8000,
                             8000,
                             8000,
                             8000,
                             8111, 8000, 8000}, sizeof(lut[95]));

    // `
    memcpy(lut[96], (int[]) {0,
                             8100,
                             8010,
                             8000,
                             8000,
                             8000,
                             8000, 8000, 8000}, sizeof(lut[96]));

    // {
    memcpy(lut[123], (int[]) {0,
                             8001,
                             8010,
                             8010,
                             8100,
                             8010,
                             8010,
                             8001, 8000}, sizeof(lut[123]));

    // |
    memcpy(lut[124], (int[]) {0,
                             8010,
                             8010,
                             8010,
                             8000,
                             8010,
                             8010,
                             8010, 8000}, sizeof(lut[124]));

    // }
    memcpy(lut[125], (int[]) {0,
                             8100,
                             8010,
                             8010,
                             8001,
                             8010,
                             8010,
                             8100, 8000}, sizeof(lut[125]));

}






int getSymbolWidth() {
    return 3;
}


int getSymbolHeight() {
    return 9;
}




int* getSymbol(char c) {
    int i = (int)c;
    return lut[i];
}






int getNthDigit(int num, int n) {
    int r, t1, t2;

    t1 = (int)powf(10, n+1);
    r = num % t1;

    if(n > 0) {
        t2 = (int)pow(10, n);
        r = r / t2;
    }

    return r;
}




void trDrawString(bitmap_t *bitmap, const char *str, int len, int x, int y, int scale) {


    int cx = x;
    int cy = y;

    // for every character in str
    for(int i=0; i<len; i++) {

        if(str[i] == 0) {
            return;
        }

        // get symbol data
        int *symbol = getSymbol(str[i]);
        int yOffset = symbol[0];

        // for every pixel of the symbol
        for(int sx=1; sx<=getSymbolWidth(); sx++) {
            for(int sy=1; sy<getSymbolHeight(); sy++) {

                // get pixel value
                int line = symbol[sy];
                int digit = getNthDigit(line, getSymbolWidth()-sx);

                // fill pixel
                for(int j=0; j<scale; j++) {
                    for(int k=0; k<scale; k++) {

                        if(digit == 1) {
                            pixel_t *pixel = bmGetPixelAt(bitmap, cx + sx*scale + j, cy + sy*scale + k + yOffset*scale);
                            if(!pixel) {
                                continue;
                            }
                            pixel->r = 1.0;
                            pixel->g = 1.0;
                            pixel->b = 1.0;
                            pixel->a = 1.0;
                        }


                    }
                }


            }
        }

        cx += getSymbolWidth()*scale + scale*2;

    }


}