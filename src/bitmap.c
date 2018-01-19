#include "bitmap.h"
#include "lodepng.h"
#include <windows.h>
#include <stdbool.h>
#include <stdio.h>



pixel_t *bmGetPixelAt(bitmap_t *bitmap, int x, int y) {
    if(x < 0) { goto error; }
    if(x >= bitmap->width) { goto error; }
    if(y < 0) { goto error; }
    if(y >= bitmap->height) { goto error; }

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
            pixel->depth = 0.0;
        }
    }
}




void bmCreate(bitmap_t *bitmap, unsigned int width, unsigned int height) {
    bitmap->pixels = calloc(width*height, sizeof(pixel_t));
    bitmap->width = width;
    bitmap->height = height;
    bitmap->scanbufferMin = calloc(height, sizeof(int));
    bitmap->scanbufferMax = calloc(height, sizeof(int));
}




void bmCreateFromPNG(bitmap_t *bitmap, char *filepath) {

    // load png
    unsigned error;
    unsigned char *image;
    unsigned w, h;
    unsigned char *png = 0;
    size_t  pngsize;
    error = lodepng_load_file(&png, &pngsize, filepath);
    if(!error) {
        error = lodepng_decode32(&image, &w, &h, png, pngsize);
    }
    if(error) {
        printf("error %u: %s\n", error, lodepng_error_text(error));
    }

    // store in bitmap
    bmCreate(bitmap, (int)w, (int)h);
    for(int y=0; y<w; y++) {
        for (int x=0; x<h; x++) {
            pixel_t *pixel = bmGetPixelAt(bitmap, x, y);
            if(pixel) {
                int r = (int)(image[4 * y * w + 4 * x + 0]);
                int g = (int)(image[4 * y * w + 4 * x + 1]);
                int b = (int)(image[4 * y * w + 4 * x + 2]);
                int a = (int)(image[4 * y * w + 4 * x + 3]);
                pixel->color.r = (float)r / 255.0f;
                pixel->color.g = (float)g / 255.0f;
                pixel->color.b = (float)b / 255.0f;
                pixel->color.a = (float)a / 255.0f;
            }
        }
    }
    // clean up png
    free(png);
    free(image);
}




void bmDispose(bitmap_t *bitmap) {
    if(bitmap->pixels) {
        free(bitmap->pixels);
        bitmap->pixels = NULL;
        free(bitmap->scanbufferMin);
        free(bitmap->scanbufferMax);
    }
}
