#ifndef SOFTWARERENDERER_SHADER_H
#define SOFTWARERENDERER_SHADER_H


#include "renderer.h"




void shaderVertex(renderdata_t *data, vertex_t *vertexIn, vertex_t *vertexOut);

void shaderFragment(renderdata_t *data, vec_t *iplVertexValuesVec3, int nVertexValuesVec3, pixel_t *pixel);




#endif
