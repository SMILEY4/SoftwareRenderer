#ifndef SOFTWARERENDERER_OBJFILE_H
#define SOFTWARERENDERER_OBJFILE_H


#include <stdbool.h>




typedef struct {
    float x;
    float y;
    float z;
} obj_vertex_t;


typedef struct {
    float u;
    float v;
    float w;
} obj_texcoord_t;


typedef struct {
    float x;
    float y;
    float z;
} obj_normal_t;


typedef struct {
    unsigned int numElements;
    int *vert_indices;
    int *tex_indices;
    int *norm_Indices;
} obj_face_t;


typedef struct {

    unsigned int nVertices;
    unsigned int nTexCoords;
    unsigned int nNormals;
    unsigned int nFaces;

    bool hasTexCoords;
    bool hasNormals;

    obj_vertex_t *vertices;
    obj_texcoord_t *texcoords;
    obj_normal_t *normals;
    obj_face_t *faces;

} obj_model_t;




bool objParse(char *filename, obj_model_t *modelOut);

bool objFree(obj_model_t *model);



#endif
