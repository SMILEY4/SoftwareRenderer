#include "bitmap.h"
#include "lodepng.h"
#include <windows.h>
#include <stdio.h>
#include <math.h>




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



pixel_t *bmFastGetPixelAt(bitmap_t *bitmap, int x, int y) {
    return bitmap->pixels + (bitmap->width*y + x);
}




pixel_t *bmGetPixelAt(bitmap_t *bitmap, int x, int y, int wrap) {

    if(wrap) {
        if (x >= bitmap->width)  { x = x % bitmap->width; }
        if (y >= bitmap->height) { y = y % bitmap->height; }
        if (x < 0) { x = x + bitmap->width; }
        if (y < 0) { y = y + bitmap->height; }
    }

    if(x < 0)               { return NULL; }
    if(x >= bitmap->width)  { return NULL; }
    if(y < 0)               { return NULL; }
    if(y >= bitmap->height) { return NULL; }

    return bitmap->pixels + (bitmap->width*y + x);
}




pixel_t *bmGetPixelUV(bitmap_t *bitmap, float u, float v, int wrap) {
    int x = (int)((float)bitmap->width  * u);
    int y = (int)((float)bitmap->height * (1.0-v));
    return bmGetPixelAt(bitmap, x, y, wrap);
}




pixel_t *bmGetPixelLongLat(bitmap_t *bitmap, float rx, float ry, float rz, int wrap) {
    const float F_PI = (float)M_PI;
    float sx = (atan2f(rz, rx) + F_PI) / (2.0f * F_PI);
    float sy = acosf(-ry) / F_PI;
    sx *= bitmap->width;
    sy *= bitmap->height;
    return bmGetPixelAt(bitmap, (int)sx, (int)sy, wrap);
}




void bmCopyBitmap(bitmap_t *dst, bitmap_t *src) {
    if(dst->width != src->width || dst->height != src->height) {
        return;
    }
    for(int y=0; y<dst->height; y++) {
        for(int x=0; x<dst->width; x++) {
            pixel_t *dstPixel = bmGetPixelAt(dst, x, y, 0);
            pixel_t *srcPixel = bmGetPixelAt(src, x, y, 0);
            dstPixel->r = srcPixel->r;
            dstPixel->g = srcPixel->g;
            dstPixel->b = srcPixel->b;
        }
    }
}




void bmDrawTo(bitmap_t *target, bitmap_t *img, float scale) {

    int w = min(target->width, img->width);
    int h = min(target->height, img->height);

    for(int x=0; x<w*scale; x++) {
        for(int y=0; y<h*scale; y++) {
            pixel_t *tgtPixel = bmGetPixelAt(target, x, y, 0);
            pixel_t *imgPixel = bmGetPixelAt(img, (int)(x/scale), (int)(y/scale), 0);
            tgtPixel->r = imgPixel->r;
            tgtPixel->g = imgPixel->g;
            tgtPixel->b = imgPixel->b;
            tgtPixel->z = imgPixel->z;
            tgtPixel->modelID = imgPixel->modelID;
            tgtPixel->triangleID = imgPixel->triangleID;
        }
    }

}




void bmClear(bitmap_t *bitmap, float r, float g, float b, float a) {
    for(int y=0; y<bitmap->height; y++) {
        for(int x=0; x<bitmap->width; x++) {
            pixel_t *pixel = bmGetPixelAt(bitmap, x, y, 0);
            pixel->r = r;
            pixel->g = g;
            pixel->b = b;
            pixel->a = a;
            pixel->z = 100000.0f;
            pixel->modelID = -1;
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
            pixel_t *pixel = bmGetPixelAt(bitmap, x, y, 0);
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
            pixel_t *pixel = bmGetPixelAt(bitmap, x, y, 0);
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




void bmCreateFromPNGCompressedHDR(bitmap_t *bitmap, char *filepath, float max) {

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
            pixel_t *pixel = bmGetPixelAt(bitmap, x, y, 0);
            *pixel = (pixel_t){0.0f, 0.0f, 0.0f, 0.0f};
            if(pixel) {
                int or = (int)(image[4 * y * w + 4 * x + 0]);
                int og = (int)(image[4 * y * w + 4 * x + 1]);
                int ob = (int)(image[4 * y * w + 4 * x + 2]);
                int oa = (int)(image[4 * y * w + 4 * x + 3]);

                int rgba = oa;
                rgba = rgba << 8;
                rgba = rgba | ob;
                rgba = rgba << 8;
                rgba = rgba | og;
                rgba = rgba << 8;
                rgba = rgba | or;

                int ib = rgba &                     0b1111111111;
                int ig = rgba &           0b11111111110000000000; ig = ig >> 10;
                int ir = rgba & 0b111111111100000000000000000000; ir = ir >> 20;

                float r = ((float)ir / 1023.0f) * max;
                float g = ((float)ig / 1023.0f) * max;
                float b = ((float)ib / 1023.0f) * max;

                pixel->r = r;
                pixel->g = g;
                pixel->b = b;

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
            pixel_t *px = bmGetPixelAt(bitmap, x, y, 0);
            float r = max(0.0f, min(px->r, 1.0f));
            float g = max(0.0f, min(px->g, 1.0f));
            float b = max(0.0f, min(px->b, 1.0f));
            float a = max(0.0f, min(px->a, 1.0f));
            image[4 * width * y + 4 * x + 0] = (unsigned char)(r*255);
            image[4 * width * y + 4 * x + 1] = (unsigned char)(g*255);
            image[4 * width * y + 4 * x + 2] = (unsigned char)(b*255);
            image[4 * width * y + 4 * x + 3] = (unsigned char)(a*255);
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
        bitmap->pixels = NULL;
        free(bitmap->pixels);
        free(bitmap->scanbufferMin);
        free(bitmap->scanbufferMax);
    }
}
