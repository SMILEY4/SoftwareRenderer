#include "shader.h"
#include "model.h"
#include "bitmap.h"
#include "camera.h"
#include "geometry.h"
#include "shaderutils.h"
#include "stopwatch.h"
#include <math.h>
#include <stdio.h>

#define DPM_HEIGHT 5.0f


// SHADOW PASS

/*
 * UNIFORM BUFFER LAYOUT
 * u0: mvp
 * */

void pshShadowPass(camera_t *camera, model_t *model, shader_t *shader, uniformbuffer_t *uniformbuffer) {
    /*
     * - calculates mvp for object
     * - write mvp in uniformbuffer (u0)
     * */
    mdlUpdateTransform(model);
    matrix_t mvp;
    matMul(&mvp, &camera->viewProjection, &model->modelTransform);
    ubSetUniform(uniformbuffer, 0, &mvp, sizeof(matrix_t));

    ubSetUniform(uniformbuffer, 1, &model->modelTransform, sizeof(matrix_t));
}




void vshShadowPass(vertex_t *vertexIn, vertex_t *vertexOut, shader_t *shader, uniformbuffer_t *uniformbuffer) {
    /*
     * - read mvp from uniformbuffer (u0)
     * - transform vertex
     * */

    vertex_t vertex = *vertexIn;

    // displacement mapping
    bitmap_t *heightmap = (bitmap_t*)ubGetPoiner(uniformbuffer, 0);
    pixel_t *pixel = bmGetPixelUV(heightmap, vertex.texCoord.x, vertex.texCoord.y, 1);
    if(pixel) {
        vec_t N = vertexIn->normal;
        vecNormalize(&N, &N);
        vecScale(&N, &N, pixel->r*DPM_HEIGHT);
        vecAdd(&vertex.position, &vertex.position, &N);
    }

    matrix_t *mvp = (matrix_t*)ubGetUniform(uniformbuffer, 0);
    matTransform(&vertexOut->position, &vertex.position, mvp);
}




void fshShadowPass(camera_t *camera, model_t *model, shader_t *shader, pixel_t *pixel, vec_t *iplUV, vec_t *iplNrm, vec_t *iplClr, vec_t *iplAttribs, uniformbuffer_t *uniformbuffer) {
    /*
     * set pixel color to fragment depth
     * */
    float d = reverseDepth(pixel->z, camera->zNear, camera->zFar) / camera->zFar;
    pixel->r = pixel->z;
    pixel->g = d;
    pixel->b = d;
    pixel->a = 1.0f;
}







// MAIN PASS

/*
 * UNIFORM BUFFER LAYOUT
 * u0: mvp
 * u1: mvp_shadow
 * u2: mdlTransform
 * p0: camera_shadow
 * p1: envmap
 * */


void pshDefault(camera_t *camera, model_t *model, shader_t *shader, uniformbuffer_t *uniformbuffer) {

    /*
     * - calculates mvp for object
     * - write mvp in uniformbuffer (u0)
     *
     * - write mdlTransform in uniformbuffer (u2)
     *
     * - read camera_shadow from uniformbuffer (p0)
     * - write mvp_shadow in uniformbuffer (u1)
     * */

    mdlUpdateTransform(model);

    matrix_t mvp;
    matMul(&mvp, &camera->viewProjection, &model->modelTransform);
    ubSetUniform(uniformbuffer, 0, &mvp, sizeof(matrix_t));

    ubSetUniform(uniformbuffer, 2, &model->modelTransform, sizeof(matrix_t));

    camera_t *cameraShadow = (camera_t*)ubGetPoiner(uniformbuffer, 0);
    matrix_t mvpShadow;
    matMul(&mvpShadow, &cameraShadow->viewProjection, &model->modelTransform);
    ubSetUniform(uniformbuffer, 1, &mvpShadow, sizeof(matrix_t));

}




void vshDefault(vertex_t *vertexIn, vertex_t *vertexOut, shader_t *shader, uniformbuffer_t *uniformbuffer) {

    /*
     * - read mvp from uniformbuffer (u0)
     * - read mdlTrans from uniformbuffer (u2)
     *
     * - transform vertex
     *
     * - transform normals
     *
     * - read camera_shadow from uniformbuffer (p0)
     * - read mvp_shadow from uniformbuffer (u1)
     *
     * */

    vertex_t vertex = *vertexIn;

    matrix_t *mvp = (matrix_t*)ubGetUniform(uniformbuffer, 0);
    matrix_t *mdlTransform = (matrix_t*)ubGetUniform(uniformbuffer, 2);


    // displacement mapping
    bitmap_t *heightmap = (bitmap_t*)ubGetPoiner(uniformbuffer, 1);
    pixel_t *pixel = bmGetPixelUV(heightmap, vertex.texCoord.x, vertex.texCoord.y, 1);
    if(pixel) {
        vec_t N = vertexIn->normal;
        vecNormalize(&N, &N);
        vecScale(&N, &N, pixel->r*DPM_HEIGHT);
        vecAdd(&vertex.position, &vertex.position, &N);
    }

    // vertex
    matTransform(&vertexOut->position, &vertex.position, mvp);
    matTransform(&vertexOut->attribs[1], &vertex.position, mdlTransform);

    // normal
    matTransform(&vertexOut->normal, &vertex.normal, mdlTransform);

    // vertex (shadow)
    camera_t *cameraShadow = (camera_t*)ubGetPoiner(uniformbuffer, 0);
    matrix_t *mvpShadow = (matrix_t*)ubGetUniform(uniformbuffer, 1);

    matTransform(&vertexOut->attribs[0], &vertex.position, mvpShadow);
    matTransform(&vertexOut->attribs[0], &vertexOut->attribs[0], &cameraShadow->screenSpaceTransform);
    vecPerspectiveDivide(&vertexOut->attribs[0], &vertexOut->attribs[0]);
}










void fshDefault_diablo(camera_t *camera, model_t *model, shader_t *shader, pixel_t *pixel, vec_t *iplUV, vec_t *iplNrm, vec_t *iplClr, vec_t *iplAttribs, uniformbuffer_t *uniformbuffer) {

    // get textures
    bitmap_t *texAlbedo     = model->textures+0;
    bitmap_t *texNormal     = model->textures+1;
    bitmap_t *texRoughness  = model->textures+2;
    bitmap_t *texAO         = model->textures+3;
    bitmap_t *texMetallic   = model->textures+4;

    // get data
    vec_t *iplPosShadow = iplAttribs+0;
    vec_t *iplWorldPos = iplAttribs+1;
    camera_t *cameraShadow = (camera_t*)ubGetPoiner(uniformbuffer, 0);
    bitmap_t *shadowmap = cameraShadow->rendertargets+0;


    // calculate light dir  (world pos -> light pow)
    vec_t L, lightDir;
    vec_t lightPos = (vec_t){cameraShadow->pos.x, cameraShadow->pos.y, cameraShadow->pos.z, 0.0f};
    vecSub(&lightDir, &lightPos, iplWorldPos);
    vecNormalize(&L, &lightDir);


    // calculate view dir
    vec_t V, viewDir;
    vecSub(&viewDir, &camera->pos, iplWorldPos);
    vecNormalize(&V, &viewDir);


    // calculate normal (+normal mapping)
    vec_t N;
    pixel_t *pxNMTangent = bmGetPixelUV(texNormal, iplUV->x, iplUV->y, 1);
    calcNormal(&N, iplNrm, &(vec_t){pxNMTangent->r, pxNMTangent->g, pxNMTangent->b, 0.0f});


    // calculate reflection vector
    vec_t R;
    vecReflect(&R, &V, &N);
    R.w = 0.0;
    vecNormalize(&R, &R);


    // shadow
    float visibility = calcShadow(&N, &L, iplPosShadow, shadowmap, cameraShadow->zNear, cameraShadow->zFar);


    // shading
    vec_t finalColor    = (vec_t){0.0f, 0.0f, 0.0f, 0.0f};

    pixel_t *pxDiffuse  = bmGetPixelUV(texAlbedo, iplUV->x, iplUV->y, 1);
    float roughness     = bmGetPixelUV(texRoughness, iplUV->x, iplUV->y, 1)->r;
    float ao            = bmGetPixelUV(texAO, iplUV->x, iplUV->y, 1)->r;
    float metalness     = bmGetPixelUV(texMetallic, iplUV->x, iplUV->y, 1)->r;
    vec_t lightColor    = (vec_t){1.3f, 1.3f, 1.3f, 0.0f};
    vec_t baseColor     = (vec_t){pxDiffuse->r, pxDiffuse->g, pxDiffuse->b, 0.0f};
    vec_t ambientColor  = (vec_t){0.2f, 0.2f, 0.2f};


    // main light
    vec_t mainLight;
    if(visibility > 0) {
        shadeDefaultLight(&mainLight, &N, &V, &L, &lightColor, &baseColor, metalness, roughness);
        vecScale(&mainLight, &mainLight, visibility);
    } else {
        mainLight = (vec_t){0.0, 0.0, 0.0, 0.0};
    }

    // ambient light
    vecMul(&ambientColor, &ambientColor, &baseColor);

    // reflection
    bitmap_t *skybox = (bitmap_t*)ubGetPoiner(uniformbuffer, 2);
    vec_t reflColor;
    calcReflection(&reflColor, &R, skybox, roughness, metalness, &baseColor, &N, &V);


    // compose sources
    vecAdd(&finalColor, &finalColor, &ambientColor);
    vecAdd(&finalColor, &finalColor, &reflColor);
    vecScale(&finalColor, &finalColor, ao);
    vecAdd(&finalColor, &finalColor, &mainLight);


    pixel->r = finalColor.r;
    pixel->g = finalColor.g;
    pixel->b = finalColor.b;
    pixel->a = 1.0;


}

























