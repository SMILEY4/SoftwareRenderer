#ifndef SOFTWARERENDERER_BITMAP_H
#define SOFTWARERENDERER_BITMAP_H

#include "geometry.h"
#include <stdbool.h>


typedef struct {
    float r, g, b, a;
} color_t;


typedef struct {
    color_t color;
    float depth;
    const unsigned int x, y;
} pixel_t;


typedef struct {
    pixel_t *pixels;
    unsigned int width;
    unsigned int height;
    int *scanbufferMin;
    int *scanbufferMax;
} bitmap_t;




pixel_t *bmGetPixelAt(bitmap_t *bitmap, int x, int y);

pixel_t *bmGetPixelUV(bitmap_t *bitmap, float u, float v);

void bmCopyBitmap(bitmap_t *dst, bitmap_t *src);

void bmDrawTo(bitmap_t *target, bitmap_t *img);

void bmClear(bitmap_t *bitmap, color_t *color);

void bmCreate(bitmap_t *bitmap, unsigned int width, unsigned int height);

void bmCreateFromPNG(bitmap_t *bitmap, char *filepath);

void bmDispose(bitmap_t *bitmap);



#endif

