#include "bitmap.h"
#include "lodepng.h"
#include <windows.h>
#include <stdio.h>
#include <float.h>




void bmSetPixel(bitmap_t *bitmap, int x, int y, float r, float g, float b) {
    if(x < 0) { return; }
    if(x >= bitmap->width) { return; }
    if(y < 0) { return; }
    if(y >= bitmap->height) { return; }

    pixel_t *pixel = bitmap->pixels + (bitmap->width*y + x);
    pixel->r = r;
    pixel->g = g;
    pixel->b = b;
}




pixel_t *bmGetPixelAt(bitmap_t *bitmap, int x, int y) {
    if(x < 0) { goto error; }
    if(x >= bitmap->width) { goto error; }
    if(y < 0) { goto error; }
    if(y >= bitmap->height) { goto error; }

    return bitmap->pixels + (bitmap->width*y + x);

    error:
        return NULL;
}




pixel_t *bmGetPixelUV(bitmap_t *bitmap, float u, float v) {
    int x = (int)((float)bitmap->width  * u);
    int y = (int)((float)bitmap->height * (1.0-v));
    return bmGetPixelAt(bitmap, x, y);
}




void bmCopyBitmap(bitmap_t *dst, bitmap_t *src) {
    if(dst->width != src->width || dst->height != src->height) {
        return;
    }
    for(int y=0; y<dst->height; y++) {
        for(int x=0; x<dst->width; x++) {
            pixel_t *dstPixel = bmGetPixelAt(dst, x, y);
            pixel_t *srcPixel = bmGetPixelAt(src, x, y);
            dstPixel->r = srcPixel->r;
            dstPixel->g = srcPixel->g;
            dstPixel->b = srcPixel->b;
        }
    }
}




void bmDrawTo(bitmap_t *target, bitmap_t *img) {

    int w = min(target->width, img->width);
    int h = min(target->height, img->height);

    for(int x=0; x<w; x++) {
        for(int y=0; y<h; y++) {
            pixel_t *tgtPixel = bmGetPixelAt(target, x, y);
            pixel_t *imgPixel = bmGetPixelAt(img, x, y);
            tgtPixel->r = imgPixel->r;
            tgtPixel->g = imgPixel->g;
            tgtPixel->b = imgPixel->b;
        }
    }

}




void bmClear(bitmap_t *bitmap, float r, float g, float b) {
    for(int y=0; y<bitmap->height; y++) {
        for(int x=0; x<bitmap->width; x++) {
            pixel_t *pixel = bmGetPixelAt(bitmap, x, y);
            pixel->r = r;
            pixel->g = g;
            pixel->b = b;
            pixel->a = 0.0f;
            pixel->z = 100000.0f;
            pixel->triangleID = -1;
        }
    }
}




void bmCreate(bitmap_t *bitmap, unsigned int width, unsigned int height) {
    bitmap->pixels = calloc(width*height, sizeof(pixel_t));
    bitmap->width = width;
    bitmap->height = height;
    bitmap->scanbufferMin = calloc(height, sizeof(int));
    bitmap->scanbufferMax = calloc(height, sizeof(int));
    for(unsigned int y=0; y<bitmap->height; y++) {
        for(unsigned int x=0; x<bitmap->width; x++) {
            pixel_t *pixel = bmGetPixelAt(bitmap, x, y);
            *pixel = (pixel_t){0.0f, 0.0f, 0.0f, 0.0f};
        }
    }
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
    for(unsigned int y=0; y<h; y++) {
        for (unsigned int x=0; x<w; x++) {
            pixel_t *pixel = bmGetPixelAt(bitmap, x, y);
            *pixel = (pixel_t){0.0f, 0.0f, 0.0f, 0.0f};
            if(pixel) {
                int r = (int)(image[4 * y * w + 4 * x + 0]);
                int g = (int)(image[4 * y * w + 4 * x + 1]);
                int b = (int)(image[4 * y * w + 4 * x + 2]);
                int a = (int)(image[4 * y * w + 4 * x + 3]);
                pixel->r = (float)r / 255.0f;
                pixel->g = (float)g / 255.0f;
                pixel->b = (float)b / 255.0f;
            }
        }
    }
    // clean up png
    free(png);
    free(image);
}




void bmSaveToFile(bitmap_t *bitmap, char *filepath) {

    int width = bitmap->width;
    int height = bitmap->height;

    unsigned char *image = malloc((size_t)(width * height * 4));
    for(int y=0; y<height; y++) {
        for(int x=0; x<width; x++) {
            pixel_t *px = bmGetPixelAt(bitmap, x, y);
            image[4 * width * y + 4 * x + 0] = (unsigned char)(px->r*255);
            image[4 * width * y + 4 * x + 1] = (unsigned char)(px->g*255);
            image[4 * width * y + 4 * x + 2] = (unsigned char)(px->b*255);
            image[4 * width * y + 4 * x + 3] = (unsigned char)(px->a*255);
        }
    }

    unsigned error = lodepng_encode32_file(filepath, image, bitmap->width, bitmap->height);
    if(error) {
        printf("error %u: %s\n", error, lodepng_error_text(error));
    }

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
