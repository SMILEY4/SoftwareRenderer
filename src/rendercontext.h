#ifndef SOFTWARERENDERERV2_RENDERCONTEXT_H
#define SOFTWARERENDERERV2_RENDERCONTEXT_H

#include "camera.h"
#include "model.h"
#include "shader.h"
#include "postprocess.h"


typedef struct {

    shader_t **shaders;
    model_t **objects;
    camera_t **cameras;
    uniformbuffer_t *buffers;
    int nObjects;

    postProcessEffect_t **ppEffects;
    int nPPEffects;

} renderdata_t;


void rcCreateRenderData(renderdata_t *renderdata, unsigned int nModels, unsigned int ubMaxElements, unsigned int ubMaxPointers, unsigned int nPPEffects);

void rcFreeRenderData(renderdata_t *renderdata);

void rcDrawModel(renderdata_t *renderdata, int dataIndex);

void rcDrawWireframeModel(renderdata_t *renderdata, int dataIndex);

void rcDrawRenderData(renderdata_t *renderdata, int drawAsWireframe);


#endif
