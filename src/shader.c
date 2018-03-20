#include "shader.h"
#include "model.h"
#include "bitmap.h"
#include <math.h>




void pshDefault(camera_t *camera, model_t *model, shader_t *shader) {
    matrix_t mvp;
    mdlUpdateTransform(model);
    matMul(&mvp, &camera->viewProjection, &model->modelTransform);
    ubSetUniform(&shader->uniforms, 0, &mvp, sizeof(matrix_t));
}




void vshDefault(vertex_t *vertexIn, vertex_t *vertexOut, shader_t *shader) {
    matrix_t *mvp = (matrix_t*)ubGetUniform(&shader->uniforms, 0);
    matTransform(&vertexOut->position, &vertexIn->position, mvp);

    matrix_t *mdlTransform = (matrix_t*)ubGetUniform(&shader->uniforms, 1);
    matTransform(&vertexOut->normal, &vertexIn->normal, mdlTransform);
}




void fshDefault(camera_t *camera, model_t *model, shader_t *shader, pixel_t *pixel, vec_t *iplPos, vec_t *iplUV, vec_t *iplNrm, vec_t *iplClr) {

    static vec_t L = {-13.6428f, 1.2000f, 13.1327f};
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

    pixel->r = NdotL * sample->r;
    pixel->g = NdotL * sample->g;
    pixel->b = NdotL * sample->b;
    pixel->a = 1.0;
}