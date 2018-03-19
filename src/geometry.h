#ifndef SOFTWARERENDERER_GEOMETRY_H
#define SOFTWARERENDERER_GEOMETRY_H


#include <stdbool.h>


typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec_t;


typedef struct {
    float m[4][4];
} matrix_t;




// VECTORS

void vecPrint(vec_t *v, char *name);

vec_t vecCreate(float x, float y, float z, float w);

void vecSet(vec_t *dst, float x, float y, float z, float w);

void vecCopy(vec_t *dst, vec_t *src);

float vecDot(vec_t *a, vec_t *b);

void vecCross(vec_t *dst, vec_t *a, vec_t *b);

float vecCross2D(vec_t *a, vec_t *b);

float vecLength(vec_t *v);

float vecDist(vec_t *a, vec_t *b);

void vecNormalize(vec_t *dst, vec_t *src);

void vecSub(vec_t *dst, vec_t *a, vec_t *b);

void vecAdd(vec_t *dst, vec_t *a, vec_t *b);

void vecMul(vec_t *dst, vec_t *a, vec_t *b);

void vecScale(vec_t *dst, vec_t *vec, float s);

void vecRotate(vec_t *dst, vec_t * vec, vec_t *axis, float angle);

float vecAngle(vec_t *a, vec_t *b);

void vecPerspectiveDivide(vec_t *dst, vec_t *src);




// MATRIX

void matPrint(matrix_t *t, char *name);

void matSetIdentity(matrix_t *mat);

void matSetScreenSpaceTransform(matrix_t *mat, float halfWidth, float halfHeight);

void matSetTranslation(matrix_t *mat, float x, float y, float z);

void matSetRotationAngle(matrix_t *mat, float x, float y, float z, float angle);

void matSetRotation(matrix_t *mat, float x, float y, float z);

void matSetRotationFUR(matrix_t *mat, vec_t *forward, vec_t *up, vec_t *right);

void matSetRotationFU(matrix_t *mat, vec_t *forward, vec_t *up);

void matSetScale(matrix_t *mat, float x, float y, float z);

void matSetPerspective(matrix_t *mat, float fov, float aspectRatio, float zNear, float zFar);

void matSetOrthographic(matrix_t *mat, float left, float right, float bottom, float top, float near, float far);

void matMul(matrix_t *dst, matrix_t *left, matrix_t *right);

void matMul3(matrix_t *dst, matrix_t *left, matrix_t *center, matrix_t *right);

void matTranspose(matrix_t *dst, matrix_t *src);

float matDet(matrix_t *mat);

void matInv(matrix_t *dst, matrix_t *src);

void matTransform(vec_t *dst, vec_t *vec, matrix_t *mat);

void matSetLookAt(matrix_t *dst, vec_t *pos, vec_t *target, vec_t *up);




// OTHERS

void barycentric(vec_t *dst, vec_t *A, vec_t *B, vec_t *C, vec_t *P);

void baryCorrectPerspective(vec_t *bary, float w1, float w2, float w3, float oow, vec_t *dst);

void interpolateBary(vec_t *dst, vec_t *A, vec_t *B, vec_t *C, vec_t *baryCoords);




#endif
