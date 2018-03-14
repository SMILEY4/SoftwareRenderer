#ifndef SOFTWARERENDERERV2_UNIFORMS_H
#define SOFTWARERENDERERV2_UNIFORMS_H

#include <windows.h>



typedef struct {
    void *data;
    size_t size;
} uniform_t;


typedef struct {
    uniform_t *uniforms;
    unsigned int maxSize;
} uniformbuffer_t;




void ubCreateBuffer(uniformbuffer_t *buffer, unsigned int maxSize);

void ubSetUniform(uniformbuffer_t *buffer, int idx, const void *data, size_t size);

void* ubGetUniform(uniformbuffer_t *buffer, int idx);

void ubFreeBuffer(uniformbuffer_t *buffer);

#endif
