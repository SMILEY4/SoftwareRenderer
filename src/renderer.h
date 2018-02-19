#ifndef SOFTWARERENDERER_RENDERER_H
#define SOFTWARERENDERER_RENDERER_H

#include "model.h"
#include "bitmap.h"
#include "camera.h"


typedef struct {

    model_t *model;
    camera_t *camera;

    void **uniformVars;
    unsigned int nUniformVars;

} renderdata_t;



void render(renderdata_t *data);




#endif
