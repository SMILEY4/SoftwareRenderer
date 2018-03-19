#include "geometry.h"

#include <math.h>
#include <stdio.h>
#include <stdbool.h>




vec_t vecCreate(float x, float y, float z, float w) {
    return (vec_t){x, y, z, w};
}




void vecSet(vec_t *dst, float x, float y, float z, float w) {
    dst->x = x;
    dst->y = y;
    dst->z = z;
    dst->w = w;
}




void vecCopy(vec_t *dst, vec_t *src) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
    dst->w = src->w;
}




float vecDot(vec_t *a, vec_t *b) {
    return a->x*b->x + a->y*b->y + a->z*b->z - a->w*b->w;
}




void vecCross(vec_t *dst, vec_t *a, vec_t *b) {
    dst->x = a->y*b->z - a->z*b->y;
    dst->y = a->z*b->x - a->x*b->z;
    dst->z = a->x*b->y - a->y*b->x;
}




float vecCross2D(vec_t *a, vec_t *b) {
    return a->x * b->y - a->y * b->x;
}




float vecLength(vec_t *v) {
    float len2 = (v->x * v->x) + (v->y * v->y) + (v->z * v->z) + (v->w * v->w);
    return sqrtf(len2);
}




float vecDist(vec_t *a, vec_t *b) {
    static vec_t tmp;
    vecSub(&tmp, a, b);
    return vecLength(&tmp);
}




void vecNormalize(vec_t *dst, vec_t *src) {
    const float len = vecLength(src);
    if(len <= 0) {
        return;
    }
    dst->x = src->x / len;
    dst->y = src->y / len;
    dst->z = src->z / len;
    dst->w = src->w / len;
}




void vecSub(vec_t *dst, vec_t *a, vec_t *b) {
    dst->x = a->x - b->x;
    dst->y = a->y - b->y;
    dst->z = a->z - b->z;
    dst->w = a->w - b->w;
}




void vecAdd(vec_t *dst, vec_t *a, vec_t *b) {
    dst->x = a->x + b->x;
    dst->y = a->y + b->y;
    dst->z = a->z + b->z;
}




void vecMul(vec_t *dst, vec_t *a, vec_t *b) {
    dst->x = a->x * b->x;
    dst->y = a->y * b->y;
    dst->z = a->z * b->z;
}




void vecScale(vec_t *dst, vec_t *vec, float s) {
    dst->x = vec->x * s;
    dst->y = vec->y * s;
    dst->z = vec->z * s;
}




void vecRotate(vec_t *dst, vec_t *vec, vec_t *axis, float angle) {
    float sinAngle = sinf(-angle);
    float cosAngle = cosf(-angle);

    vec_t v0, v1;
    vecScale(&v0, axis, sinAngle);
    vecScale(&v1, vec, cosAngle);

    vec_t v2;
    vecCross(&v2, vec, &v0);

    vec_t v3;
    vecScale(&v3, axis, 1-cosAngle);

    float f0 = vecDot(vec, &v3);

    vec_t v4;
    vecScale(&v4, axis, f0);

    vec_t v5;
    vecAdd(&v5, &v1, &v4);

    vecAdd(dst, &v2, &v5);
}




float vecAngle(vec_t *a, vec_t *b) {
    const float lenAdd = sqrtf( (a->x+b->x)*(a->x+b->x) + (a->y+b->y)*(a->y+b->y) + (a->z+b->z)*(a->z+b->z) );
    const float lenSub = sqrtf( (a->x-b->x)*(a->x-b->x) + (a->y-b->y)*(a->y-b->y) + (a->z-b->z)*(a->z-b->z) );
    float angleRad = 2.0f * atanf(lenSub / lenAdd);
    return (float)((angleRad) * 180.0 / M_PI);
}




void vecPerspectiveDivide(vec_t *dst, vec_t *src) {
    dst->x = src->x / src->w;
    dst->y = src->y / src->w;
    dst->z = src->z / src->w;
    dst->w = src->w;
//    dst->w = src->w / src->w;
}




void vecPrint(vec_t *v, char *name) {
    printf("%s: vec3(%10.4f; %10.4f; %10.4f; %10.4f)\n", name, v->x, v->y, v->z, v->w);
}






void matSetIdentity(matrix_t *mat) {
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            if(i == j) {
                mat->m[i][j] = 1.0;
            } else {
                mat->m[i][j] = 0.0;
            }
        }
    }
}




void matSetScreenSpaceTransform(matrix_t *mat, float halfWidth, float halfHeight) {
    mat->m[0][0] = halfWidth;           mat->m[0][1] = 0;                   mat->m[0][2] = 0;                   mat->m[0][3] = halfWidth;
    mat->m[1][0] = 0;                   mat->m[1][1] = -halfHeight;         mat->m[1][2] = 0;                   mat->m[1][3] = halfHeight;
    mat->m[2][0] = 0;                   mat->m[2][1] = 0;                   mat->m[2][2] = 1;                   mat->m[2][3] = 0;
    mat->m[3][0] = 0;                   mat->m[3][1] = 0;                   mat->m[3][2] = 0;                   mat->m[3][3] = 1;
}




void matSetTranslation(matrix_t *mat, float x, float y, float z) {
    matSetIdentity(mat);
    mat->m[0][3] = x;
    mat->m[1][3] = y;
    mat->m[2][3] = z;
}




void matSetRotationAngle(matrix_t *mat, float x, float y, float z, float angle) {
    float s = sinf(angle);
    float c = cosf(angle);
    mat->m[0][0] = c+x*x*(1-c);         mat->m[0][1] = x*y*(1-c)-z*s;        mat->m[0][2] = x*z*(1-c)+y*s;      mat->m[0][3] = 0;
    mat->m[1][0] = y*x*(1-c)+z*s;       mat->m[1][1] = c+y*y*(1-c);          mat->m[1][2] = y*z*(1-c)-x*s;      mat->m[1][3] = 0;
    mat->m[2][0] = z*x*(1-c)-y*s;       mat->m[2][1] = z*y*(1-c)+x*s;        mat->m[2][2] = c+z*z*(1-c);        mat->m[2][3] = 0;
    mat->m[3][0] = 0;                   mat->m[3][1] = 0;                    mat->m[3][2] = 0;                  mat->m[3][3] = 1;
}




void matSetRotation(matrix_t *mat, float x, float y, float z) {

    matrix_t rx, ry, rz;

    rz.m[0][0] = cosf(z);            rz.m[0][1] = -sinf(z);           rz.m[0][2] = 0;                 rz.m[0][3] = 0;
    rz.m[1][0] = sinf(z);            rz.m[1][1] = cosf(z);            rz.m[1][2] = 0;                 rz.m[1][3] = 0;
    rz.m[2][0] = 0;                 rz.m[2][1] = 0;                 rz.m[2][2] = 1;                 rz.m[2][3] = 0;
    rz.m[3][0] = 0;                 rz.m[3][1] = 0;                 rz.m[3][2] = 0;                 rz.m[3][3] = 1;

    rx.m[0][0] = 1;                 rx.m[0][1] = 0;                 rx.m[0][2] = 0;                 rx.m[0][3] = 0;
    rx.m[1][0] = 0;                 rx.m[1][1] = cosf(x);            rx.m[1][2] = -sinf(x);           rx.m[1][3] = 0;
    rx.m[2][0] = 0;                 rx.m[2][1] = sinf(x);            rx.m[2][2] = cosf(x);            rx.m[2][3] = 0;
    rx.m[3][0] = 0;                 rx.m[3][1] = 0;                 rx.m[3][2] = 0;                 rx.m[3][3] = 1;

    ry.m[0][0] = cosf(y);            ry.m[0][1] = 0;                 ry.m[0][2] = -sinf(y);           ry.m[0][3] = 0;
    ry.m[1][0] = 0;                 ry.m[1][1] = 1;                 ry.m[1][2] = 0;                 ry.m[1][3] = 0;
    ry.m[2][0] = sinf(y);            ry.m[2][1] = 0;                 ry.m[2][2] = cosf(y);            ry.m[2][3] = 0;
    ry.m[3][0] = 0;                 ry.m[3][1] = 0;                 ry.m[3][2] = 0;                 ry.m[3][3] = 1;

    matrix_t ryx;
    matMul(&ryx, &ry, &rx);
    matMul(mat, &rz, &ryx);
}



void matSetRotationFUR(matrix_t *mat, vec_t *forward, vec_t *up, vec_t *right) {

    vec_t *f = forward;
    vec_t *u = up;
    vec_t *r = right;

    mat->m[0][0] = r->x;              mat->m[0][1] = r->y;              mat->m[0][2] = r->z;              mat->m[0][3] = 0;
    mat->m[1][0] = u->x;              mat->m[1][1] = u->y;              mat->m[1][2] = u->z;              mat->m[1][3] = 0;
    mat->m[2][0] = f->x;              mat->m[2][1] = f->y;              mat->m[2][2] = f->z;              mat->m[2][3] = 0;
    mat->m[3][0] = 0;                 mat->m[3][1] = 0;                 mat->m[3][2] = 0;                 mat->m[3][3] = 1;

}




void matSetRotationFU(matrix_t *mat, vec_t *forward, vec_t *up) {

    vec_t f, r, u;

    vecNormalize(&f, forward);
    vecNormalize(&r, up);

    vecCross(&r, &r, &f);
    vecCross(&u, &f, &r);

    matSetRotationFUR(mat, &f, &u, &r);
}




void matSetScale(matrix_t *mat, float x, float y, float z) {
    matSetIdentity(mat);
    mat->m[0][0] = x;
    mat->m[1][1] = y;
    mat->m[2][2] = z;
}




void matSetPerspective(matrix_t *mat, float fov, float aspectRatio, float zNear, float zFar) {
    float tanHalfFOV = tanf(fov/2.0f);
    float zRange = zNear - zFar;
    mat->m[0][0] = 1.0f/(tanHalfFOV*aspectRatio); mat->m[0][1] = 0;                            mat->m[0][2] = 0;                            mat->m[0][3] = 0;
    mat->m[1][0] = 0;                            mat->m[1][1] = 1.0f/tanHalfFOV;              mat->m[1][2] = 0;                            mat->m[1][3] = 0;
    mat->m[2][0] = 0;                            mat->m[2][1] = 0;                            mat->m[2][2] = (-zNear -zFar)/zRange;        mat->m[2][3] = 2 * zFar * zNear / zRange;
    mat->m[3][0] = 0;                            mat->m[3][1] = 0;                            mat->m[3][2] = 1;                            mat->m[3][3] = 0;
}




void matSetOrthographic(matrix_t *mat, float left, float right, float bottom, float top, float near, float far) {
    float width = right - left;
    float height = top - bottom;
    float depth = far - near;

    mat->m[0][0] = 2/width;           mat->m[0][1] = 0;                 mat->m[0][2] = 0;                 mat->m[0][3] = -(right + left)/width;
    mat->m[1][0] = 0;                 mat->m[1][1] = 2/height;          mat->m[1][2] = 0;                 mat->m[1][3] = -(top + bottom)/height;
    mat->m[2][0] = 0;                 mat->m[2][1] = 0;                 mat->m[2][2] = -2/depth;          mat->m[2][3] = -(far + near)/depth;
    mat->m[3][0] = 0;                 mat->m[3][1] = 0;                 mat->m[3][2] = 0;                 mat->m[3][3] = 1;
}




void matMul(matrix_t *dst, matrix_t *left, matrix_t *right) {
    static matrix_t tmp;

    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            tmp.m[i][j] =   left->m[i][0] * right->m[0][j] +
                            left->m[i][1] * right->m[1][j] +
                            left->m[i][2] * right->m[2][j] +
                            left->m[i][3] * right->m[3][j];
        }
    }

    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            dst->m[i][j] = tmp.m[i][j];
        }
    }

}




void matMul3(matrix_t *dst, matrix_t *left, matrix_t *center, matrix_t *right) {
    matMul(dst, center, right);
    matMul(dst, left, dst);
}




void matTranspose(matrix_t *dst, matrix_t *src) {
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            dst->m[i][j] = src->m[j][i];
        }
    }
}



float matDet(matrix_t *mat) {
    return
          mat->m[0][3] * mat->m[2][2] * mat->m[1][1] * mat->m[0][0]
        - mat->m[0][2] * mat->m[3][3] * mat->m[1][1] * mat->m[0][0]
        - mat->m[0][3] * mat->m[1][1] * mat->m[2][2] * mat->m[0][0]
        + mat->m[0][1] * mat->m[3][3] * mat->m[2][2] * mat->m[0][0]
        + mat->m[0][2] * mat->m[1][1] * mat->m[3][3] * mat->m[0][0]
        - mat->m[0][1] * mat->m[2][2] * mat->m[3][3] * mat->m[0][0]
        - mat->m[0][3] * mat->m[2][2] * mat->m[0][0] * mat->m[1][1]
        + mat->m[0][2] * mat->m[3][3] * mat->m[0][0] * mat->m[1][1]
        + mat->m[0][3] * mat->m[0][0] * mat->m[2][2] * mat->m[1][1]
        - mat->m[0][0] * mat->m[3][3] * mat->m[2][2] * mat->m[1][1]
        - mat->m[0][2] * mat->m[0][0] * mat->m[3][3] * mat->m[1][1]
        + mat->m[0][0] * mat->m[2][2] * mat->m[3][3] * mat->m[1][1]
        + mat->m[0][3] * mat->m[1][1] * mat->m[0][0] * mat->m[2][2]
        - mat->m[0][1] * mat->m[3][3] * mat->m[0][0] * mat->m[2][2]
        - mat->m[0][3] * mat->m[0][0] * mat->m[1][1] * mat->m[2][2]
        + mat->m[0][0] * mat->m[3][3] * mat->m[1][1] * mat->m[2][2]
        + mat->m[0][1] * mat->m[0][0] * mat->m[3][3] * mat->m[2][2]
        - mat->m[0][0] * mat->m[1][1] * mat->m[3][3] * mat->m[2][2]
        - mat->m[0][2] * mat->m[1][1] * mat->m[0][0] * mat->m[3][3]
        + mat->m[0][1] * mat->m[2][2] * mat->m[0][0] * mat->m[3][3]
        + mat->m[0][2] * mat->m[0][0] * mat->m[1][1] * mat->m[3][3]
        - mat->m[0][0] * mat->m[2][2] * mat->m[1][1] * mat->m[3][3]
        - mat->m[0][1] * mat->m[0][0] * mat->m[2][2] * mat->m[3][3]
        + mat->m[0][0] * mat->m[1][1] * mat->m[2][2] * mat->m[3][3];
}




void matTransform(vec_t *dst, vec_t *vec, matrix_t *mat) {
    float rx = mat->m[0][0] * vec->x + mat->m[0][1] * vec->y + mat->m[0][2] * vec->z + mat->m[0][3] * vec->w;
    float ry = mat->m[1][0] * vec->x + mat->m[1][1] * vec->y + mat->m[1][2] * vec->z + mat->m[1][3] * vec->w;
    float rz = mat->m[2][0] * vec->x + mat->m[2][1] * vec->y + mat->m[2][2] * vec->z + mat->m[2][3] * vec->w;
    float rw = mat->m[3][0] * vec->x + mat->m[3][1] * vec->y + mat->m[3][2] * vec->z + mat->m[3][3] * vec->w;
    dst->x = rx;
    dst->y = ry;
    dst->z = rz;
    dst->w = rw;
}




void matPrint(matrix_t *t, char *name) {
    printf("%s:\n", name);
    printf("[%10.4f; %10.4f; %10.4f; %10.4f]\n", t->m[0][0], t->m[0][1], t->m[0][2], t->m[0][3]);
    printf("[%10.4f; %10.4f; %10.4f; %10.4f]\n", t->m[1][0], t->m[1][1], t->m[1][2], t->m[1][3]);
    printf("[%10.4f; %10.4f; %10.4f; %10.4f]\n", t->m[2][0], t->m[2][1], t->m[2][2], t->m[2][3]);
    printf("[%10.4f; %10.4f; %10.4f; %10.4f]\n", t->m[3][0], t->m[3][1], t->m[3][2], t->m[3][3]);
}




void matSetLookAt(matrix_t *dst, vec_t *pos, vec_t *target, vec_t *up) {

    vec_t xaxis, yaxis, zaxis;

//    vecSub(&zaxis, pos, target);
    vecSub(&zaxis, target, pos);
    vecNormalize(&zaxis, &zaxis);

    vecCross(&xaxis, up, &zaxis);
    vecNormalize(&xaxis, &xaxis);

    vecCross(&yaxis, &zaxis, &xaxis);

    matrix_t orientation;
    orientation.m[0][0] = xaxis.x;        orientation.m[0][1] = xaxis.y;        orientation.m[0][2] = xaxis.z;        orientation.m[0][3] = 0;
    orientation.m[1][0] = yaxis.x;        orientation.m[1][1] = yaxis.y;        orientation.m[1][2] = yaxis.z;        orientation.m[1][3] = 0;
    orientation.m[2][0] = zaxis.x;        orientation.m[2][1] = zaxis.y;        orientation.m[2][2] = zaxis.z;        orientation.m[2][3] = 0;
    orientation.m[3][0] = 0;              orientation.m[3][1] = 0;              orientation.m[3][2] = 0;              orientation.m[3][3] = 1;

    matrix_t translation;
    translation.m[0][0] = 1;              translation.m[0][1] = 0;              translation.m[0][2] = 0;              translation.m[0][3] = -pos->x;
    translation.m[1][0] = 0;              translation.m[1][1] = 1;              translation.m[1][2] = 0;              translation.m[1][3] = -pos->y;
    translation.m[2][0] = 0;              translation.m[2][1] = 0;              translation.m[2][2] = 1;              translation.m[2][3] = -pos->z;
    translation.m[3][0] = 0;              translation.m[3][1] = 0;              translation.m[3][2] = 0;              translation.m[3][3] = 1;

    matMul(dst, &orientation, &translation);

}





void barycentric(vec_t *dst, vec_t *A, vec_t *B, vec_t *C, vec_t *P) {
    vec_t V1, V2, Q;
    vecSub(&V1, B, A);
    vecSub(&V2, C, A);
    vecSub(&Q , P, A);
    dst->y = vecCross2D(&Q, &V2) / vecCross2D(&V1, &V2);
    dst->z = vecCross2D(&V1, &Q) / vecCross2D(&V1, &V2);
    dst->x = 1.0f - dst->y -  dst->z;
}


void baryCorrectPerspective(vec_t *bary, float w1, float w2, float w3, float oow, vec_t *dst) {
    static vec_t p1 = {1, 0, 0, 0};
    static vec_t p2 = {0, 1, 0, 0};
    static vec_t p3 = {0, 0, 1, 0};
    float x = ( ((bary->x/w1)*p1.x) + ((bary->y/w2)*p2.x) + ((bary->z/w3)*p3.x) ) / oow;
    float y = ( ((bary->x/w1)*p1.y) + ((bary->y/w2)*p2.y) + ((bary->z/w3)*p3.y) ) / oow;
    float z = ( ((bary->x/w1)*p1.z) + ((bary->y/w2)*p2.z) + ((bary->z/w3)*p3.z) ) / oow;
    dst->x = x;
    dst->y = y;
    dst->z = z;
}



void interpolateBary(vec_t *dst, vec_t *A, vec_t *B, vec_t *C, vec_t *baryCoords) {
    dst->x = A->x * baryCoords->x  +  B->x * baryCoords->y  +  C->x * baryCoords->z;
    dst->y = A->y * baryCoords->x  +  B->y * baryCoords->y  +  C->y * baryCoords->z;
    dst->z = A->z * baryCoords->x  +  B->z * baryCoords->y  +  C->z * baryCoords->z;
    dst->w = A->w * baryCoords->x  +  B->w * baryCoords->y  +  C->w * baryCoords->z;
}