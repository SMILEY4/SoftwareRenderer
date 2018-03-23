#include "uniforms.h"
#include <stdio.h>


void ubCreateBuffer(uniformbuffer_t *buffer, unsigned int ubMaxElements, unsigned int maxPointers) {
    buffer->maxSize = ubMaxElements;
    buffer->uniforms = calloc((size_t)ubMaxElements, sizeof(uniform_t));
    for(int i=0; i<ubMaxElements; i++) {
        uniform_t *uniform = buffer->uniforms+i;
        uniform->data = NULL;
        uniform->size = 0;
    }
    buffer->maxPointers = maxPointers;
    buffer->pointers = calloc((size_t)maxPointers, sizeof(void*));
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
        if(uniform->size == size) {
            memcpy(uniform->data, data, size);

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
}




void* ubGetUniform(uniformbuffer_t *buffer, int idx) {
    if(idx < 0 || idx >= buffer->maxSize) {
        return NULL;
    }
    return buffer->uniforms[idx].data;
}




void ubSetPointer(uniformbuffer_t *buffer, int ptrIdx, void *ptr) {
    if(ptrIdx < 0 || ptrIdx >= buffer->maxPointers) {
        return;
    }
    buffer->pointers[ptrIdx] = ptr;
}




void* ubGetPoiner(uniformbuffer_t *buffer, int ptrIdx) {
    if(ptrIdx < 0 || ptrIdx >= buffer->maxPointers) {
        return NULL;
    }
    return buffer->pointers[ptrIdx];
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
