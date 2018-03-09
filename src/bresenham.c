#include "bresenham.h"
#include "bitmap.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>




void bhDrawLineToScanbuffer(int *scanbufferMin, int *scanbufferMax, int scanbufferSize, int ax, int ay, int bx, int by) {

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

        if(0 <= currentY && currentY < scanbufferSize) {
            scanbufferMin[currentY] = (int)fmin(scanbufferMin[currentY], currentX);
            scanbufferMax[currentY] = (int)fmaxf(scanbufferMax[currentY], currentX);
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

        pixel_t *pixel = bmGetPixelAt(bitmap, currentX, currentY);
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

