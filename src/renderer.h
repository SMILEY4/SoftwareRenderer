#ifndef SOFTWARERENDERER_RENDERER_H
#define SOFTWARERENDERER_RENDERER_H

#include "model.h"
#include "bitmap.h"
#include "camera.h"


typedef struct {

    model_t *model;
    camera_t *camera;
    bitmap_t *renderTargets;
    unsigned int nRenderTargets;

    vertex_t *verticesV; // after vertex-shader
    vertex_t *verticesS; // in screen-space (may contain uninitialized elements)
    int *discard; // 0 = draw vertex; 1=discard vertex
    unsigned int nVertices;

    int nVSArgs;
    int nFSArgs;
    void **vsArgs;
    void **fsArgs;

} renderdata_t;



void render(renderdata_t *data);




#endif
