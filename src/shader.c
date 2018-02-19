#include "geometry.h"
#include "shader.h"
#include "renderer.h"

#include <math.h>
#include <stdio.h>





void shaderVertex(renderdata_t *data, vertex_t *vertexIn, vertex_t *vertexOut) {
    void **uniformVars = data->uniformVars;
    matrix_t *mvp = (matrix_t*)(*(uniformVars+0));
    matrix_t *nrmTransform = (matrix_t*)(*(uniformVars+1));
    matTransform(&vertexOut->valuesVec3[0], &vertexIn->valuesVec3[0], mvp);
    matTransform(&vertexOut->valuesVec3[2], &vertexIn->valuesVec3[2], nrmTransform);
    vecNormalize(&vertexOut->valuesVec3[2], &vertexOut->valuesVec3[2]);
}




void shaderFragment(renderdata_t *data, vec_t *iplVertexValuesVec3, int nVertexValuesVec3, pixel_t *pixel) {

    void **uniformVars = data->uniformVars;
    bitmap_t *renderTargets = data->camera->rendertargets;

    // get interpolated values
    vec_t iplPos = iplVertexValuesVec3[0];
    vec_t iplUV = iplVertexValuesVec3[1];
    vec_t iplNormal = iplVertexValuesVec3[2];

    // calc shading
    vec_t lightDir = (vec_t){0.5, 1.0, 0.7};
    vecNormalize(&iplNormal, &iplNormal);
    vecNormalize(&lightDir, &lightDir);
    float NdotL = vecDot(&iplNormal, &lightDir);
    NdotL = fminf(fmaxf(NdotL, 0.0f), 1.0f);

    // set final color
    pixel->color.r = NdotL + 0.1f;
    pixel->color.g = NdotL + 0.1f;
    pixel->color.b = NdotL + 0.1f;

}


