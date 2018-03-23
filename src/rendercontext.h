#ifndef SOFTWARERENDERERV2_RENDERCONTEXT_H
#define SOFTWARERENDERERV2_RENDERCONTEXT_H

#include "camera.h"
#include "model.h"
#include "shader.h"


int g_pickedTriangle;

typedef struct {
    shader_t **shaders;
    model_t **objects;
    camera_t **cameras;
    uniformbuffer_t *buffers;
    int nObjects;
} renderdata_t;


void rcCreateRenderData(renderdata_t *renderdata, int nModels, unsigned int ubMaxElements, unsigned int ubMaxPointers);

void rcFreeRenderData(renderdata_t *renderdata);

void rcDrawModel(camera_t *camera, model_t *model, shader_t *shader, uniformbuffer_t *uniformbuffer);

void rcDrawRenderData(renderdata_t *renderdata);

#endif
