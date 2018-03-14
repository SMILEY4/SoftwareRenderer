#ifndef SOFTWARERENDERERV2_SHADER_H
#define SOFTWARERENDERERV2_SHADER_H

#include "geometry.h"
#include "model.h"
#include "uniforms.h"

typedef struct shader_t shader_t;

struct shader_t {
    void (*vsh)(vertex_t *vertexIn, vertex_t *vertexOut, shader_t *shader);
    void (*fsh)(shader_t *shader);
    uniformbuffer_t uniforms;
};


void vshDefault(vertex_t *vertexIn, vertex_t *vertexOut, shader_t *shader);

void fshDefault(shader_t *shader);

#endif



