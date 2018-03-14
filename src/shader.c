#include "shader.h"
#include "model.h"


void vshDefault(vertex_t *vertexIn, vertex_t *vertexOut, shader_t *shader) {
    matrix_t *mvp = (matrix_t*)ubGetUniform(&shader->uniforms, 0);
    matTransform(&vertexOut->position, &vertexIn->position, mvp);

    matrix_t *mdlTransform = (matrix_t*)ubGetUniform(&shader->uniforms, 1);
    matTransform(&vertexOut->normal, &vertexIn->normal, mdlTransform);
}




void fshDefault(shader_t *shader) {
}