#include "postprocess.h"

#include "geometry.h"
#include "bitmap.h"




void ppPassThrough(pixel_t *pixelIn, vec_t *colorOut) {
    colorOut->r = pixelIn->r;
    colorOut->g = pixelIn->g;
    colorOut->b = pixelIn->b;
    colorOut->a = pixelIn->a;
}