#ifndef SOFTWARERENDERERV2_UNIFORMS_H
#define SOFTWARERENDERERV2_UNIFORMS_H

#include "bitmap.h"
#include <windows.h>



typedef struct {
    void *data;
    size_t size;
} uniform_t;


typedef struct {

    // uniform data
    uniform_t *uniforms;
    unsigned int maxSize;

    // texture data
    void **pointers;
    unsigned int maxPointers;

} uniformbuffer_t;




void ubCreateBuffer(uniformbuffer_t *buffer, unsigned int maxSize, unsigned int maxPointers);

void ubSetUniform(uniformbuffer_t *buffer, int idx, const void *data, size_t size);

void* ubGetUniform(uniformbuffer_t *buffer, int idx);

void ubSetPointer(uniformbuffer_t *buffer, int ptrIdx, void *ptr);

void* ubGetPoiner(uniformbuffer_t *buffer, int ptrIdx);

void ubFreeBuffer(uniformbuffer_t *buffer);

#endif
