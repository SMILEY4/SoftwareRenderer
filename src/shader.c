#include "geometry.h"
#include "shader.h"
#include "renderer.h"
#include "bitmap.h"
#include "camera.h"

#include <math.h>
#include <stdio.h>





void shaderVertex_main(renderdata_t *data, vertex_t *vertexIn, vertex_t *vertexOut) {
    void **uniformVars = data->uniformVars;
    matrix_t *mvp = (matrix_t*)(*(uniformVars+0));
    matrix_t *nrmTransform = (matrix_t*)(*(uniformVars+1));
    matTransform(&vertexOut->valuesVec3[0], &vertexIn->valuesVec3[0], mvp);
    matTransform(&vertexOut->valuesVec3[2], &vertexIn->valuesVec3[2], nrmTransform);
    vecNormalize(&vertexOut->valuesVec3[2], &vertexOut->valuesVec3[2]);
}




void shaderFragment_main(renderdata_t *data, vec_t *iplVertexValuesVec3, int nVertexValuesVec3, pixel_t *pixel) {

    void **uniformVars = data->uniformVars;
    bitmap_t *renderTargets = data->camera->rendertargets;
    matrix_t *nrmTransform = (matrix_t*)(*(uniformVars+1));


    // get light dir
    vec_t lightdir;
    vec_t *lightpos = (vec_t*)uniformVars[2];
    vecScale(&lightdir, lightpos, -1.0f);
    float lightDist = vecLength(&lightdir);
    vecNormalize(&lightdir, &lightdir);



    // get interpolated values
    vec_t iplPos = iplVertexValuesVec3[0];
    vec_t iplUV = iplVertexValuesVec3[1];
    vec_t iplNormal = iplVertexValuesVec3[2];


    // get view dir
    vec_t viewdir;
    vecSub(&viewdir, &data->camera->pos, &iplPos);
    vecNormalize(&viewdir, &viewdir);


    // get texture colors
    pixel_t *pxDiffuse =    bmGetPixelUV(&data->model->textures[0], iplUV.x, iplUV.y);
    pixel_t *pxNM =         bmGetPixelUV(&data->model->textures[1], iplUV.x, iplUV.y);
    pixel_t *pxNMTangent =  bmGetPixelUV(&data->model->textures[2], iplUV.x, iplUV.y);
    pixel_t *pxSpec =       bmGetPixelUV(&data->model->textures[3], iplUV.x, iplUV.y);
    pixel_t *pxSSS =        bmGetPixelUV(&data->model->textures[4], iplUV.x, iplUV.y);


    // calculate normal
    vec_t nrmVert = iplNormal;
    vec_t nrmNM = {pxNM->color.r, pxNM->color.g, pxNM->color.b};
    vec_t nrmNMT = {pxNMTangent->color.r, pxNMTangent->color.g, pxNMTangent->color.b};

    vec_t minusOne = {-1, -1, -1};
    vecNormalize(&nrmVert, &nrmVert);
    vecScale(&nrmNM, &nrmNM, -2.0f);   vecSub(&nrmNM, &nrmNM, &minusOne);   vecNormalize(&nrmNM, &nrmNM); // world space normal, transformed

    vec_t N;
    matTransform(&N, &nrmNM, nrmTransform);
    vecNormalize(&N, &N);

    // calc shading
    float NdotL = vecDot(&N, &lightdir);
    NdotL = fminf(fmaxf(NdotL, 0.0f), 1.0f) + 0.4f;

    // set final color
    pixel->color.r = NdotL * pxDiffuse->color.r;
    pixel->color.g = NdotL * pxDiffuse->color.g;
    pixel->color.b = NdotL * pxDiffuse->color.b;

}


