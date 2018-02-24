#include "debugRenderer.h"
#include "bitmap.h"
#include "renderer.h"
#include "geometry.h"
#include "bresenham.h"
#include <math.h>




void dbDrawLine(bitmap_t *target, int x0, int y0, int x1, int y1, color_t *color) {
    bhDrawLine(target, x0, y0, x1, y1, *color);
}




void dbDrawDisk(bitmap_t *target, int cx, int cy, int radius, color_t *color) {
    for(int x=(int)fmaxf(cx-radius, 0); x<(int)fminf(cx+radius, target->width); x++) {
        for(int y=(int)fmaxf(cy-radius, 0); y<(int)fminf(cy+radius, target->height); y++) {
            const float dist = sqrtf((x-cx)*(x-cx) + (y-cy)*(y-cy));
            if(dist <= radius) {
                pixel_t *pixel = bmGetPixelAt(target, x, y);
                pixel->color = (color_t){color->r, color->g, color->b, 1.0f};
            }
        }
    }
}














