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
    bitmap_t **textures;
    unsigned int maxTextures;

} uniformbuffer_t;




void ubCreateBuffer(uniformbuffer_t *buffer, unsigned int maxSize, unsigned int maxTextures);

void ubSetUniform(uniformbuffer_t *buffer, int idx, const void *data, size_t size);

void* ubGetUniform(uniformbuffer_t *buffer, int idx);

void ubSetTexture(uniformbuffer_t *buffer, int texIdx, bitmap_t *bitmap);

bitmap_t* ubGetTexture(uniformbuffer_t *buffer, int texIdx);

void ubFreeBuffer(uniformbuffer_t *buffer);

#endif
