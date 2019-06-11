
#ifndef SOFTWARERENDERER_POSTPROCESS_H
#define SOFTWARERENDERER_POSTPROCESS_H

#include "geometry.h"
#include "bitmap.h"

typedef struct {
    void (*ppFx)(pixel_t *pixelIn, vec_t *colorOut);
} postProcessEffect_t;


void ppPassThrough(pixel_t *pixelIn, vec_t *colorOut);

#endif
