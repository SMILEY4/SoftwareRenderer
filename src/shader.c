#include "geometry.h"
#include "shader.h"

#include <math.h>





void shaderVertex(vertex_t *vertexIn, vertex_t *vertexOut, void **uniformVars) {
    void *varg0 = *(uniformVars+0);
    matrix_t *mvp = (matrix_t*)varg0;
    matTransform(&vertexOut->valuesVec3[0], &vertexIn->valuesVec3[0], mvp);
}




void shaderFragment(vec_t *iplVertexValuesVec3, int nVertexValuesVec3, pixel_t *pixel, void **uniformVars) {

    // get values
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
    pixel->color.r = NdotL * iplNormal.x;
    pixel->color.g = NdotL * iplNormal.y;
    pixel->color.b = NdotL * iplNormal.z;

}


