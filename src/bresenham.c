#include "bresenham.h"
#include "bitmap.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>



void bhDrawLineToScanbuffer(int *scanbufferMin, int *scanbufferMax, int scanbufferSize, int x0, int y0, int x1, int y1) {

    const int dx = abs(x1-x0);
    const int dy = abs(y1-y0);

    const int sx = x0 < x1 ? 1 : -1;
    const int sy = y0 < y1 ? 1 : -1;

    int err = dx - dy;
    int e2;
    int currentX = x0;
    int currentY = y0;

    while(true) {

        if(0 <= currentY && currentY < scanbufferSize) {
            scanbufferMin[currentY] = min(scanbufferMin[currentY], currentX);
            scanbufferMax[currentY] = max(scanbufferMax[currentY], currentX);
        }

        if(currentX == x1 && currentY == y1) {
            break;
        }

        e2 = 2*err;
        if(e2 > -1 * dy) {
            err = err - dy;
            currentX = currentX + sx;
        }
        if(e2 < dx) {
            err = err + dx;
            currentY = currentY + sy;
        }
    }


}



void bhDrawLine(bitmap_t *bitmap, int ax, int ay, int bx, int by, float r, float g, float b) {
    if(!bitmap) {
        return;
    }

    int x0 = ax;
    int y0 = ay;
    int x1 = bx;
    int y1 = by;

    int dx = (int)fabsf(x1-x0);
    int dy = (int)fabsf(y1-y0);

    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;

    int err = dx - dy;
    int e2;
    int currentX = x0;
    int currentY = y0;

    while(true) {

        pixel_t *pixel = bmGetPixelAt(bitmap, currentX, currentY, 0);
        if(pixel) {
            pixel->r = r;
            pixel->g = g;
            pixel->b = b;
            pixel->a = 1.0;
        }

        if(currentX == x1 && currentY == y1) {
            break;
        }

        e2 = 2*err;
        if(e2 > -1 * dy) {
            err = err - dy;
            currentX = currentX + sx;
        }
        if(e2 < dx) {
            err = err + dx;
            currentY = currentY + sy;
        }
    }

}

