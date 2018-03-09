#ifndef SOFTWARERENDERER_MODEL_H
#define SOFTWARERENDERER_MODEL_H


#include "objfile.h"
#include "geometry.h"
#include "bitmap.h"
#include "camera.h"


typedef struct triangle_t triangle_t;
typedef struct model_t model_t;
typedef struct vertex_t vertex_t;



struct vertex_t {
    vec_t position;
    vec_t normal;
    vec_t texCoord;
};


struct triangle_t {
    vertex_t *vertices;
};

struct model_t {
    vec_t translation, rotation, scale;
    matrix_t matTranslation, matRotation, matScale;
    matrix_t modelTransform;

    triangle_t *triangles;
    unsigned int nTriangles;
    bitmap_t *textures;
};



void mdlUpdateTransform(model_t *model);

void mdlCreateFromObj(obj_model_t *objmodel, model_t *model, char **textureFiles, unsigned int nTextures, unsigned int nAdditionalVertexValues);

void mdlFreeModel(model_t *model);




#endif
