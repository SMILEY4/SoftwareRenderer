#include "uniforms.h"
#include <stdio.h>


void ubCreateBuffer(uniformbuffer_t *buffer, unsigned int maxSize) {
    buffer->maxSize = maxSize;
    buffer->uniforms = calloc((size_t)maxSize, sizeof(uniform_t));
    for(int i=0; i<maxSize; i++) {
        uniform_t *uniform = buffer->uniforms+i;
        uniform->data = NULL;
        uniform->size = 0;
    }
}




void ubSetUniform(uniformbuffer_t *buffer, int idx, const void *data, size_t size) {
    if(idx < 0 || idx >= buffer->maxSize) {
        return;
    }

    uniform_t *uniform = buffer->uniforms+idx;

    if(!data) {
        free(uniform->data);
        uniform->size = 0;

    } else {
        if(uniform->size > 0) {
            free(uniform->data);
            uniform->size = 0;
        }
        uniform->data = malloc(size);
        uniform->size = size;
        memcpy(uniform->data, data, size);
    }
}




void* ubGetUniform(uniformbuffer_t *buffer, int idx) {
    if(idx < 0 || idx >= buffer->maxSize) {
        return NULL;
    }
    return buffer->uniforms[idx].data;
}




void ubFreeBuffer(uniformbuffer_t *buffer) {
    for(int i=0; i<buffer->maxSize; i++) {
        uniform_t *uniform = buffer->uniforms+i;
        if(uniform->size > 0) {
            free(uniform->data);
        }
    }
    free(buffer->uniforms);
    buffer->maxSize = 0;
}
