#include "geometry.h"
#include "shader.h"
#include "renderer.h"
#include "bitmap.h"
#include "camera.h"

#include <math.h>
#include <stdio.h>







void shaderVertex_shadow(renderdata_t *data, int indexModel, vertex_t *vertexIn, vertex_t *vertexOut) {
    void **uniformVars = data->uniformVars;
    matrix_t *mvp = (matrix_t*)(*(uniformVars+0));
    matTransform(&vertexOut->valuesVec3[0], &vertexIn->valuesVec3[0], mvp);
}


void shaderFragment_shadow(renderdata_t *data, int indexModel, vec_t *iplVertexValuesVec3, int nVertexValuesVec3, pixel_t *pixel) {
    vec_t iplPos = iplVertexValuesVec3[0];
    pixel->color.r = iplPos.z;
    pixel->color.g = iplPos.z;
    pixel->color.b = iplPos.z;
}








void shaderVertex_main(renderdata_t *data, int indexModel, vertex_t *vertexIn, vertex_t *vertexOut) {
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



void shaderFragment_main(renderdata_t *data, int indexModel, vec_t *iplVertexValuesVec3, int nVertexValuesVec3, pixel_t *pixel) {

    void **uniformVars = data->uniformVars;
    bitmap_t *renderTargets = data->camera->rendertargets;
    matrix_t *nrmTransform = (matrix_t*)(*(uniformVars+1));

    // get interpolated values
    vec_t iplPos = iplVertexValuesVec3[0];
    vec_t iplUV = iplVertexValuesVec3[1];
    vec_t iplNormal = iplVertexValuesVec3[2];
    vec_t iplShadowCoord = iplVertexValuesVec3[3];
    vecNormalize(&iplNormal, &iplNormal);




    vec_t L = {-0.888565f, -0.438497f, -0.134805f};
    vec_t V = {data->camera->pos.x, data->camera->pos.y, data->camera->pos.z, 0.0};  vecNormalize(&V, &V);

    // pos mapping
    pixel_t *pxNMTangent =  bmGetPixelUV(&data->models[indexModel].textures[2], iplUV.x, iplUV.y);
    vec_t Nm = (vec_t){pxNMTangent->color.r*2.0f - 1.0f, pxNMTangent->color.g*2.0f - 1.0f, pxNMTangent->color.b*2.0f - 1.0f, 0.0f};

    vec_t Nv = iplNormal;
    vec_t Nv2 = {Nv.x+4, Nv.y+4, Nv.z+4, 1.0};
    vec_t Tv; vecCross(&Tv, &Nv, &Nv2);
    vec_t Bv; vecCross(&Bv, &Nv, &Tv);

    Nv.w = 0.0;  vecNormalize(&Nv, &Nv);
    Tv.w = 0.0;  vecNormalize(&Tv, &Tv);
    Bv.w = 0.0;  vecNormalize(&Bv, &Bv);

    matrix_t TBN, TBN0;
    TBN.m[0][0] = Tv.x;    TBN.m[0][1] = Tv.y;    TBN.m[0][2] = Tv.z;    TBN.m[0][3] = 0.0f;
    TBN.m[1][0] = Bv.x;    TBN.m[1][1] = Bv.y;    TBN.m[1][2] = Bv.z;    TBN.m[1][3] = 0.0f;
    TBN.m[2][0] = Nv.x;    TBN.m[2][1] = Nv.y;    TBN.m[2][2] = Nv.z;    TBN.m[2][3] = 0.0f;
    TBN.m[3][0] = 0.0f;    TBN.m[3][1] = 0.0f;    TBN.m[3][2] = 0.0f;    TBN.m[3][3] = 1.0f;
    matTranspose(&TBN0, &TBN);

    vec_t N;
    matTransform(&N, &Nm, &TBN0);
    vecNormalize(&N, &N);



    // shadow
    float visibility = 1.0;

    float cosTheta = vecDot(&N, &L);
    float bias = fmaxf(0.05f * (1.0f - cosTheta), 0.005f);
    bias = clamp(bias, 0.0f, 0.01f);

    bitmap_t *shadowMap = (bitmap_t*)(*(uniformVars+3));
    pixel_t *shadowPX = bmGetPixelAt(shadowMap, (unsigned int)iplShadowCoord.x, (unsigned int)iplShadowCoord.y);
    if(shadowPX && shadowPX->color.r < iplShadowCoord.z-bias) {
        visibility = 0.0;
    }




    // blinn shading
    vec_t ambient = {0.2f, 0.2f, 0.27f, 0.0f};
    vec_t lightColor = {1.0f, 1.0f, 1.0f, 1.2f};

    pixel_t *pxDiffuse = bmGetPixelUV(&data->models[indexModel].textures[0], iplUV.x, iplUV.y);

    float lambertian = fmaxf(vecDot(&N, &L), 0.0f);
    float specular = 0.0f;


    specular *= visibility;
    lambertian *= visibility;

    vec_t finalColor = {
            ambient.x * pxDiffuse->color.r  +  pxDiffuse->color.r * lambertian * lightColor.x*lightColor.w  +  specular * lightColor.x*lightColor.w,
            ambient.y * pxDiffuse->color.r  +  pxDiffuse->color.g * lambertian * lightColor.y*lightColor.w  +  specular * lightColor.y*lightColor.w,
            ambient.z * pxDiffuse->color.r  +  pxDiffuse->color.b * lambertian * lightColor.z*lightColor.w  +  specular * lightColor.z*lightColor.w,
            0.0f
    };


    // final color
    pixel->color.r = finalColor.x;
    pixel->color.g = finalColor.y;
    pixel->color.b = finalColor.z;


    pixel->color.r = 0.0;
    pixel->color.g = 1.0;
    pixel->color.b = 0.0;


    bitmap_t *skybox = (bitmap_t*)(*(uniformVars+5));

    vec_t R = {Nv.x, Nv.y, Nv.z, 0.0}; // reflection vector ( calculate with reflect(...) )
    vecNormalize(&R, &R);


    float sx, sy;
    const float F_PI = (float)M_PI;

    sx = (atan2f(R.z, R.x) + F_PI) / (2.0f*F_PI);
    sy = acosf(-R.y) / F_PI;


    sx *= skybox->width;
    sy *= skybox->height;

    pixel_t *pxSky = bmGetPixelAt(skybox, (int)sx, (int)sy);
    if(!pxSky) {
        return;
    }

    pixel->color.r = pxSky->color.r;
    pixel->color.g = pxSky->color.g;
    pixel->color.b = pxSky->color.b;



}

















