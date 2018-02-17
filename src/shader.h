#ifndef SOFTWARERENDERER_SHADER_H
#define SOFTWARERENDERER_SHADER_H


#include "renderer.h"




void shaderVertex(vertex_t *vertexIn, vertex_t *vertexOut, void **uniformVars);

void shaderFragment(vec_t *iplVertexValuesVec3, int nVertexValuesVec3, pixel_t *pixel, void **uniformVars);




#endif
