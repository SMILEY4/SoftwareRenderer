#include "bresenham.h"
#include "bitmap.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>




void bhDrawLine(bitmap_t *bitmap, int ax, int ay, int bx, int by, color_t color) {
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
            pixel->color.r = color.r * color.a;
            pixel->color.g = color.g * color.a;
            pixel->color.b = color.b * color.a;
            pixel->color.a = 1.0;
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




void bhDrawTriangle(bitmap_t *bitmap, int ax, int ay, int bx, int by, int cx, int cy, color_t ca, color_t cb, color_t cc) {
    bhDrawLine(bitmap, ax, ay, bx, by, ca);
    bhDrawLine(bitmap, bx, by, cx, cy, cb);
    bhDrawLine(bitmap, cx, cy, ax, ay, cc);
}