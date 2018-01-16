#ifndef SOFTWARERENDERER_GEOMETRY_H
#define SOFTWARERENDERER_GEOMETRY_H


#include <stdbool.h>


typedef struct {
    double x;
    double y;
    double z;
    double w;
} vec_t;


typedef struct {
    double m[4][4];
} matrix_t;




// VECTORS

void vecPrint(vec_t *v, char *name);

vec_t vecCreate(double x, double y, double z, double w);

void vecSet(vec_t *dst, double x, double y, double z, double w);

void vecCopy(vec_t *dst, vec_t *src);

double vecDot(vec_t *a, vec_t *b);

void vecCross(vec_t *dst, vec_t *a, vec_t *b);

double vecLength(vec_t *v);

double vecDist(vec_t *a, vec_t *b);

void vecNormalize(vec_t *dst, vec_t *src);

void vecSub(vec_t *dst, vec_t *a, vec_t *b);

void vecAdd(vec_t *dst, vec_t *a, vec_t *b);

void vecMul(vec_t *dst, vec_t *a, vec_t *b);

void vecScale(vec_t *dst, vec_t *vec, double s);

void vecRotate(vec_t *dst, vec_t * vec, vec_t *axis, double angle);

void vecPerspectiveDivide(vec_t *dst, vec_t *src);




// MATRIX

void matPrint(matrix_t *t, char *name);

void matSetIdentity(matrix_t *mat);

void matSetScreenSpaceTransform(matrix_t *mat, double halfWidth, double halfHeight);

void matSetTranslation(matrix_t *mat, double x, double y, double z);

void matSetRotationAngle(matrix_t *mat, double x, double y, double z, double angle);

void matSetRotation(matrix_t *mat, double x, double y, double z);

void matSetRotationFUR(matrix_t *mat, vec_t *forward, vec_t *up, vec_t *right);

void matSetRotationFU(matrix_t *mat, vec_t *forward, vec_t *up);

void matSetScale(matrix_t *mat, double x, double y, double z);

void matSetPerspective(matrix_t *mat, double fov, double aspectRatio, double zNear, double zFar);

void matSetOrthographic(matrix_t *mat, float left, float right, float bottom, float top, float near, float far);

void matMul(matrix_t *dst, matrix_t *left, matrix_t *right);

void matMul3(matrix_t *dst, matrix_t *left, matrix_t *center, matrix_t *right);

void matTransform(vec_t *dst, vec_t *vec, matrix_t *mat);

void matSetLookAt(matrix_t *dst, vec_t *pos, vec_t *target, vec_t *up);


#endif
