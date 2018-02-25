#ifndef SOFTWARERENDERER_SHADER_H
#define SOFTWARERENDERER_SHADER_H


#include "renderer.h"



void shaderVertex_shadow(renderdata_t *data, int indexModel, vertex_t *vertexIn, vertex_t *vertexOut);

void shaderFragment_shadow(renderdata_t *data, int indexModel, vec_t *iplVertexValuesVec3, int nVertexValuesVec3, pixel_t *pixel);




void shaderVertex_main(renderdata_t *data, int indexModel, vertex_t *vertexIn, vertex_t *vertexOut);

void shaderFragment_main(renderdata_t *data, int indexModel, vec_t *iplVertexValuesVec3, int nVertexValuesVec3, pixel_t *pixel);




#endif
