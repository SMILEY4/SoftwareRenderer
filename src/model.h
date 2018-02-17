#ifndef SOFTWARERENDERER_MODEL_H
#define SOFTWARERENDERER_MODEL_H


#include "objfile.h"
#include "geometry.h"
#include "bitmap.h"




typedef struct triangle_t triangle_t;
typedef struct model_t model_t;
typedef struct vertex_t vertex_t;



struct vertex_t {
    unsigned int vertex_id;
    triangle_t *parent;
    vec_t *valuesVec3;
};



struct triangle_t {
    unsigned int triangle_id;
    model_t *parent;
    vertex_t *vertices;
};



struct model_t {
    vec_t translation, rotation, scale;
    matrix_t matTranslation, matRotation, matScale;
    matrix_t modelTransform;
    
    triangle_t *triangles;
    unsigned int nTriangles;
    unsigned int nVertValuesVec3;
    bitmap_t texture;
};



void mdlUpdateTransform(model_t *model);

void mdlCreateFromObj(obj_model_t *objmodel, model_t *model, char *fileTexture, unsigned int nAdditionalVertexValues);

void mdlFreeModel(model_t *model);




#endif
