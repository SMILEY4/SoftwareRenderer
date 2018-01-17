#ifndef SOFTWARERENDERER_BITMAP_H
#define SOFTWARERENDERER_BITMAP_H


#include <stdbool.h>


typedef struct {
    float r, g, b, a;
} color_t;


typedef struct {
    color_t color;
    double depth;
} pixel_t;


typedef struct {
    pixel_t *pixels;
    unsigned int width;
    unsigned int height;
} bitmap_t;




pixel_t *bmGetPixelAt(bitmap_t *bitmap, int x, int y);

void bmCopyBitmap(bitmap_t *dst, bitmap_t *src);

void bmClear(bitmap_t *bitmap, color_t *color);

void bmCreate(bitmap_t *bitmap, unsigned int width, unsigned int height);

void bmCreateFromPNG(bitmap_t *bitmap, char *filepath);

void bmDispose(bitmap_t *bitmap);



#endif

