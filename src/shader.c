#include "geometry.h"
#include "shader.h"
#include "renderer.h"
#include "bitmap.h"
#include "camera.h"

#include <math.h>
#include <stdio.h>







void shaderVertex_shadow(renderdata_t *data, vertex_t *vertexIn, vertex_t *vertexOut) {
    void **uniformVars = data->uniformVars;
    matrix_t *mvp = (matrix_t*)(*(uniformVars+0));
    matTransform(&vertexOut->valuesVec3[0], &vertexIn->valuesVec3[0], mvp);
}


void shaderFragment_shadow(renderdata_t *data, vec_t *iplVertexValuesVec3, int nVertexValuesVec3, pixel_t *pixel) {
    vec_t iplPos = iplVertexValuesVec3[0];
    pixel->color.r = iplPos.z;
    pixel->color.g = iplPos.z;
    pixel->color.b = iplPos.z;
}








void shaderVertex_main(renderdata_t *data, vertex_t *vertexIn, vertex_t *vertexOut) {
    void **uniformVars = data->uniformVars;

    // vertex
    matrix_t *mvp = (matrix_t*)(*(uniformVars+0));
    matTransform(&vertexOut->valuesVec3[0], &vertexIn->valuesVec3[0], mvp);

    // vertex (shadow)
    matrix_t *mvpShadow = (matrix_t*)(*(uniformVars+4));
    matTransform(&vertexOut->valuesVec3[3], &vertexIn->valuesVec3[0], mvpShadow);

    float z = -vertexOut->valuesVec3[3].z;

    static matrix_t screenSpaceTransform;
    matSetScreenSpaceTransform(&screenSpaceTransform, 512.0f/2.0f, 512.0f/2.0f);

    matTransform(&vertexOut->valuesVec3[3], &vertexOut->valuesVec3[3], &screenSpaceTransform);
    vecPerspectiveDivide(&vertexOut->valuesVec3[3], &vertexOut->valuesVec3[3]);

    vertexOut->valuesVec3[3].z = (z-0.5f)/(100.0f-0.5f);


    // normals
    matrix_t *nrmTransform = (matrix_t*)(*(uniformVars+1));
    matTransform(&vertexOut->valuesVec3[2], &vertexIn->valuesVec3[2], nrmTransform);
    vecNormalize(&vertexOut->valuesVec3[2], &vertexOut->valuesVec3[2]);
}



float clamp(float x, float a, float b) {
    return fminf(fmaxf(a, x), b);
}



void shaderFragment_main(renderdata_t *data, vec_t *iplVertexValuesVec3, int nVertexValuesVec3, pixel_t *pixel) {

    void **uniformVars = data->uniformVars;
    bitmap_t *renderTargets = data->camera->rendertargets;
    matrix_t *nrmTransform = (matrix_t*)(*(uniformVars+1));

    // get light dir
    vec_t lightpos = (vec_t){-23.506086f, -11.599998f, -3.566123f, 1.0000f};
    vec_t lightdir;
    vecScale(&lightdir, &lightpos, 1.0f);
    lightdir.w = 1.0;
    vecNormalize(&lightdir, &lightdir);





    // get interpolated values
    vec_t iplPos = iplVertexValuesVec3[0];
    vec_t iplUV = iplVertexValuesVec3[1];
    vec_t iplNormal = iplVertexValuesVec3[2];
    vec_t iplShadowCoord = iplVertexValuesVec3[3];



    // get view dir
    vec_t viewdir;
    vecSub(&viewdir, &data->camera->pos, &iplPos);
    vecNormalize(&viewdir, &viewdir);



    // get texture colors
    pixel_t *pxDiffuse =    bmGetPixelUV(&data->model->textures[0], iplUV.x, iplUV.y);
    pixel_t *pxNM =         bmGetPixelUV(&data->model->textures[1], iplUV.x, iplUV.y);
    pixel_t *pxNMTangent =  bmGetPixelUV(&data->model->textures[2], iplUV.x, iplUV.y);
    pixel_t *pxTex3 =       bmGetPixelUV(&data->model->textures[3], iplUV.x, iplUV.y);
    pixel_t *pxTex4 =       bmGetPixelUV(&data->model->textures[4], iplUV.x, iplUV.y);




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
    float NdotL = vecDot(&nrmVert, &lightdir);
    NdotL = fminf(fmaxf(NdotL, 0.0f), 1.0f);


    // shadow calculations
    float visibility = 1.0;

    float cosTheta = vecDot(&N, &lightdir);
    float bias = fmaxf(0.05f * (1.0f - cosTheta), 0.005f);
    bias = clamp(bias, 0.0f, 0.01f);

    bitmap_t *shadowMap = (bitmap_t*)(*(uniformVars+3));
    pixel_t *shadowPX = bmGetPixelAt(shadowMap, (unsigned int)iplShadowCoord.x, (unsigned int)iplShadowCoord.y);

    if(shadowPX->color.r < iplShadowCoord.z-bias) {
        visibility = 0.0;
    }


    // set final color
    pixel->color.r = NdotL * visibility * pxDiffuse->color.r  +  pxDiffuse->color.r * pxTex4->color.r;
    pixel->color.g = NdotL * visibility * pxDiffuse->color.g  +  pxDiffuse->color.g * pxTex4->color.g;
    pixel->color.b = NdotL * visibility * pxDiffuse->color.b  +  pxDiffuse->color.b * pxTex4->color.b;

}


