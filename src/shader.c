#include "shader.h"
#include "model.h"
#include "bitmap.h"
#include "camera.h"
#include "geometry.h"
#include <math.h>
#include <stdio.h>



// SHADOW PASS

void pshShadowPass(camera_t *camera, model_t *model, shader_t *shader) {
    matrix_t mvp;
    mdlUpdateTransform(model);
    matMul(&mvp, &camera->viewProjection, &model->modelTransform);
    ubSetUniform(&shader->uniforms, 0, &mvp, sizeof(matrix_t));
}




void vshShadowPass(vertex_t *vertexIn, vertex_t *vertexOut, shader_t *shader) {
    matrix_t *mvp = (matrix_t*)ubGetUniform(&shader->uniforms, 0);
    matTransform(&vertexOut->position, &vertexIn->position, mvp);
}




void fshShadowPass(camera_t *camera, model_t *model, shader_t *shader, pixel_t *pixel, vec_t *iplPos, vec_t *iplUV, vec_t *iplNrm, vec_t *iplClr, vec_t *iplAttribs) {
    float d = reverseDepth(iplPos->z, 0.1f, 100.0f) / 100.0f;
    pixel->r = iplPos->z;
    pixel->g = d;
    pixel->b = d;
    pixel->a = 1.0f;
}







// MAIN PASS

void pshDefault(camera_t *camera, model_t *model, shader_t *shader) {
    mdlUpdateTransform(model);

    matrix_t mvp;
    matMul(&mvp, &camera->viewProjection, &model->modelTransform);
    ubSetUniform(&shader->uniforms, 0, &mvp, sizeof(matrix_t));

    camera_t *cameraShadow = (camera_t*)ubGetPoiner(&shader->uniforms, 0);
    matrix_t mvpShadow;
    matMul(&mvpShadow, &cameraShadow->viewProjection, &model->modelTransform);
    ubSetUniform(&shader->uniforms, 2, &mvpShadow, sizeof(matrix_t));

}




void vshDefault(vertex_t *vertexIn, vertex_t *vertexOut, shader_t *shader) {

    // vertex
    matrix_t *mvp = (matrix_t*)ubGetUniform(&shader->uniforms, 0);
    matTransform(&vertexOut->position, &vertexIn->position, mvp);

    // normal
    matrix_t *mdlTransform = (matrix_t*)ubGetUniform(&shader->uniforms, 1);
    matTransform(&vertexOut->normal, &vertexIn->normal, mdlTransform);

    // vertex (shadow)
    camera_t *cameraShadow = (camera_t*)ubGetPoiner(&shader->uniforms, 0);
    matrix_t *mvpShadow = (matrix_t*)ubGetUniform(&shader->uniforms, 2);

    matTransform(&vertexOut->attribs[0], &vertexIn->position, mvpShadow);
    matTransform(&vertexOut->attribs[0], &vertexOut->attribs[0], &cameraShadow->screenSpaceTransform);
    vecPerspectiveDivide(&vertexOut->attribs[0], &vertexOut->attribs[0]);

}




void fshDefault(camera_t *camera, model_t *model, shader_t *shader, pixel_t *pixel, vec_t *iplPos, vec_t *iplUV, vec_t *iplNrm, vec_t *iplClr, vec_t *iplAttribs) {

    static vec_t L = {-14.6018f, 4.0f, 17.6737f, 0.0f};
    L.w = 0.0;
    vecNormalize(&L, &L);

    vec_t N = {iplNrm->x, iplNrm->y, iplNrm->z, 0.0};
    N.w = 0.0;
    vecNormalize(&N, &N);


    // shading
    float NdotL = vecDot(&N, &L);
    NdotL = fmaxf(0.1f, fminf(NdotL, 1.0f));



    // sample texture
    pixel_t *sample = bmGetPixelUV(model->textures+0, iplUV->x, iplUV->y);
    if(!sample) {
        pixel->r = NdotL;
        pixel->g = NdotL;
        pixel->b = NdotL;
        return;
    }

    // http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/#exponential-shadow-maps
    // sample shadowmap
    vec_t iplPosShadow = iplAttribs[0];
    camera_t *cameraShadow = (camera_t*)ubGetPoiner(&shader->uniforms, 0);
    bitmap_t *shadowmap = cameraShadow->rendertargets+0;
    pixel_t *shadowPX = bmGetPixelAt(shadowmap, (unsigned int)iplPosShadow.x, (unsigned int)iplPosShadow.y);

    float visibility = 1.0f;
    float cosTheta = vecDot(&N, &L);

    if(shadowPX) {
        float zShadow = reverseDepth(iplPosShadow.z, camera->zNear, camera->zFar); // TODO: use shadowCam here ?
        float zPixel = reverseDepth(shadowPX->r, camera->zNear, camera->zFar);
//        float bias = fmaxf(1.2f * (1.0f - cosTheta), 0.05f);
        if(zPixel < zShadow) {
            visibility = 0.2f;
        }

    } else {
        visibility = 1.0f;
    }

    if(cosTheta < 0) {
        visibility = 0.2f;
    }

    pixel->r = NdotL * sample->r * visibility + 0.2f;
    pixel->g = NdotL * sample->g * visibility + 0.2f;
    pixel->b = NdotL * sample->b * visibility + 0.2f;
    pixel->a = 1.0;


//    pixel->r = iplPosShadow.x/400.0f;
//    pixel->g = iplPosShadow.y/400.0f;;
//    pixel->b = iplPosShadow.z/400.0f;;
//    pixel->a = 1.0;

}