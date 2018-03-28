#ifndef SOFTWARERENDERER_BITMAP_H
#define SOFTWARERENDERER_BITMAP_H

#include <stdbool.h>


typedef struct {
    float r;
    float g;
    float b;
    float a;
    float z;
    int modelID;
    int triangleID;
} pixel_t;


typedef struct {
    pixel_t *pixels;
    unsigned int width;
    unsigned int height;
    int *scanbufferMin;
    int *scanbufferMax;
} bitmap_t;




void bmSetPixel(bitmap_t *bitmap, int x, int y, float r, float g, float b);

pixel_t *bmFastGetPixelAt(bitmap_t *bitmap, int x, int y);

pixel_t *bmGetPixelAt(bitmap_t *bitmap, int x, int y, int wrap);

pixel_t *bmGetPixelUV(bitmap_t *bitmap, float u, float v, int wrap);

pixel_t *bmGetPixelLongLat(bitmap_t *bitmap, float rx, float ry, float rz, int wrap);

void bmCopyBitmap(bitmap_t *dst, bitmap_t *src);

void bmDrawTo(bitmap_t *target, bitmap_t *img, float scale);

void bmClear(bitmap_t *bitmap, float r, float g, float b, float a);

void bmCreate(bitmap_t *bitmap, unsigned int width, unsigned int height);

void bmCreateFromPNG(bitmap_t *bitmap, char *filepath);

void bmSaveToFile(bitmap_t *bitmap, char *filepath);

void bmDispose(bitmap_t *bitmap);



#endif

