#include "bitmap.h"
#include <windows.h>
#include <stdbool.h>




pixel_t *bmGetPixelAt(bitmap_t *bitmap, int x, int y) {
    if(x < 0) { goto error; }
    if(x > bitmap->width) { goto error; }
    if(y < 0) { goto error; }
    if(y > bitmap->height) { goto error; }

    return bitmap->pixels + (bitmap->width*y + x);

    error:
        return NULL;
}




void bmCopyBitmap(bitmap_t *dst, bitmap_t *src) {
    if(dst->width != src->width || dst->height != src->height) {
        return;
    }
    for(int y=0; y<dst->height; y++) {
        for(int x=0; x<dst->width; x++) {
            pixel_t *dstPixel = bmGetPixelAt(dst, x, y);
            pixel_t *srcPixel = bmGetPixelAt(src, x, y);
            dstPixel->color.r = srcPixel->color.r;
            dstPixel->color.g = srcPixel->color.g;
            dstPixel->color.b = srcPixel->color.b;
            dstPixel->color.a = srcPixel->color.a;
        }
    }
}




void bmClear(bitmap_t *bitmap, color_t *color) {
    for(int y=0; y<bitmap->height; y++) {
        for(int x=0; x<bitmap->width; x++) {
            pixel_t *pixel = bmGetPixelAt(bitmap, x, y);
            pixel->color.r = color->r;
            pixel->color.g = color->g;
            pixel->color.b = color->b;
            pixel->color.a = color->a;
        }
    }
}




void bmCreate(bitmap_t *bitmap, unsigned int width, unsigned int height) {
    bitmap->pixels = calloc(width*height, sizeof(pixel_t));
    bitmap->width = width;
    bitmap->height = height;
}




void bmDispose(bitmap_t *bitmap) {
    if(bitmap->pixels) {
        free(bitmap->pixels);
        bitmap->pixels = NULL;
    }
}
