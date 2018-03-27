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
    const float len2 = (v->x * v->x) + (v->y * v->y) + (v->z * v->z) + (v->w * v->w);
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
    const float sinAngle = sinf(-angle);
    const float cosAngle = cosf(-angle);

    vec_t v0, v1;
    vecScale(&v0, axis, sinAngle);
    vecScale(&v1, vec, cosAngle);

    vec_t v2;
    vecCross(&v2, vec, &v0);

    vec_t v3;
    vecScale(&v3, axis, 1-cosAngle);

    const float f0 = vecDot(vec, &v3);

    vec_t v4;
    vecScale(&v4, axis, f0);

    vec_t v5;
    vecAdd(&v5, &v1, &v4);

    vecAdd(dst, &v2, &v5);
}




float vecAngle(vec_t *a, vec_t *b) {
    const float lenAdd = sqrtf( (a->x+b->x)*(a->x+b->x) + (a->y+b->y)*(a->y+b->y) + (a->z+b->z)*(a->z+b->z) );
    const float lenSub = sqrtf( (a->x-b->x)*(a->x-b->x) + (a->y-b->y)*(a->y-b->y) + (a->z-b->z)*(a->z-b->z) );
    const float angleRad = 2.0f * atanf(lenSub / lenAdd);
    return (float)((angleRad) * 180.0 / M_PI);
}




void vecReflect(vec_t *dst, vec_t *i, vec_t *n) {
    const float NdotI = vecDot(n, i);
    const float x = i->x - 2.0f * NdotI * n->x;
    const float y = i->y - 2.0f * NdotI * n->y;
    const float z = i->z - 2.0f * NdotI * n->z;
    dst->x = x;
    dst->y = y;
    dst->z = z;
}




void vecPerspectiveDivide(vec_t *dst, vec_t *src) {
    const float w = src->w;
    dst->x = src->x / w;
    dst->y = src->y / w;
    dst->z = src->z / w;
    dst->w = w;
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
    const float s = sinf(angle);
    const float c = cosf(angle);
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
    const float tanHalfFOV = tanf(fov/2.0f);
    const float zRange = zNear - zFar;
    mat->m[0][0] = 1.0f/(tanHalfFOV*aspectRatio);mat->m[0][1] = 0;                            mat->m[0][2] = 0;                            mat->m[0][3] = 0;
    mat->m[1][0] = 0;                            mat->m[1][1] = 1.0f/tanHalfFOV;              mat->m[1][2] = 0;                            mat->m[1][3] = 0;
    mat->m[2][0] = 0;                            mat->m[2][1] = 0;                            mat->m[2][2] = (-zNear -zFar)/zRange;        mat->m[2][3] = 2 * zFar * zNear / zRange;
    mat->m[3][0] = 0;                            mat->m[3][1] = 0;                            mat->m[3][2] = 1;                            mat->m[3][3] = 0;
}




void matSetInvPerspective(matrix_t *mat, float fov, float aspectRatio, float zNear, float zFar) {
    const float tanHalfFOV = tanf(fov/2.0f);
    const float zRange = zNear - zFar;
    mat->m[0][0] = tanHalfFOV*aspectRatio;       mat->m[0][1] = 0;                            mat->m[0][2] = 0;                            mat->m[0][3] = 0;
    mat->m[1][0] = 0;                            mat->m[1][1] = tanHalfFOV;                   mat->m[1][2] = 0;                            mat->m[1][3] = 0;
    mat->m[2][0] = 0;                            mat->m[2][1] = 0;                            mat->m[2][2] = 0;                            mat->m[2][3] = 1;
    mat->m[3][0] = 0;                            mat->m[3][1] = 0;                            mat->m[3][2] = zRange/(2.0f*zFar*zNear);     mat->m[3][3] = (zFar+zNear)/(2.0f*zFar*zNear);
}




void matSetOrthographic(matrix_t *mat, float left, float right, float bottom, float top, float near, float far) {
    const float width = right - left;
    const float height = top - bottom;
    const float depth = far - near;

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

    float f =   mat->m[0][0] * ( (mat->m[1][1]*mat->m[2][2]*mat->m[3][3] + mat->m[1][2]*mat->m[2][3]*mat->m[3][1] + mat->m[1][3]*mat->m[2][1]*mat->m[3][2])
                               - mat->m[1][3] * mat->m[2][2] * mat->m[3][1]
                               - mat->m[1][1] * mat->m[2][3] * mat->m[3][2]
                               - mat->m[1][2] * mat->m[2][1] * mat->m[3][3]
                             );

    f -=        mat->m[0][1] * ( (mat->m[1][0]*mat->m[2][2]*mat->m[3][3] + mat->m[1][2]*mat->m[2][3]*mat->m[3][0] + mat->m[1][3]*mat->m[2][0]*mat->m[3][2])
                               - mat->m[1][3] * mat->m[2][2] * mat->m[3][0]
                               - mat->m[1][0] * mat->m[2][3] * mat->m[3][2]
                               - mat->m[1][2] * mat->m[2][0] * mat->m[3][3]
                               );

    f +=        mat->m[0][2] * ( (mat->m[1][0]*mat->m[2][1]*mat->m[3][3] + mat->m[1][1]*mat->m[2][3]*mat->m[3][0] + mat->m[1][3]*mat->m[2][0]*mat->m[3][1])
                                 - mat->m[1][3] * mat->m[2][1] * mat->m[3][0]
                                 - mat->m[1][0] * mat->m[2][3] * mat->m[3][1]
                                 - mat->m[1][1] * mat->m[2][0] * mat->m[3][3]
                               );

    f -=        mat->m[0][3] * ( (mat->m[1][0]*mat->m[2][1]*mat->m[3][2] + mat->m[1][1]*mat->m[2][2]*mat->m[3][0] + mat->m[1][2]*mat->m[2][0]*mat->m[3][1])
                                 - mat->m[1][2] * mat->m[2][1] * mat->m[3][0]
                                 - mat->m[1][0] * mat->m[2][2] * mat->m[3][1]
                                 - mat->m[1][1] * mat->m[2][0] * mat->m[3][2]
    );
    
    return f;
}




void matTransform(vec_t *dst, vec_t *vec, matrix_t *mat) {
    const float x = vec->x;
    const float y = vec->y;
    const float z = vec->z;
    const float w = vec->w;
    const float rx = mat->m[0][0] * x + mat->m[0][1] * y + mat->m[0][2] * z + mat->m[0][3] * w;
    const float ry = mat->m[1][0] * x + mat->m[1][1] * y + mat->m[1][2] * z + mat->m[1][3] * w;
    const float rz = mat->m[2][0] * x + mat->m[2][1] * y + mat->m[2][2] * z + mat->m[2][3] * w;
    const float rw = mat->m[3][0] * x + mat->m[3][1] * y + mat->m[3][2] * z + mat->m[3][3] * w;
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



float det3x3(float t00, float t01, float t02,
             float t10, float t11, float t12,
             float t20, float t21, float t22)
{
    return   t00 * (t11 * t22 - t12 * t21)
             + t01 * (t12 * t20 - t10 * t22)
             + t02 * (t10 * t21 - t11 * t20);
}




void matInv(matrix_t *dst, matrix_t *src) {
    const float det = matDet(src);
    if(det == 0) {
        return;
    }

    const float t00 =  det3x3(src->m[1][1], src->m[1][2], src->m[1][3], src->m[2][1], src->m[2][2], src->m[2][3], src->m[3][1], src->m[3][2], src->m[3][3]);
    const float t01 = -det3x3(src->m[1][0], src->m[1][2], src->m[1][3], src->m[2][0], src->m[2][2], src->m[2][3], src->m[3][0], src->m[3][2], src->m[3][3]);
    const float t02 =  det3x3(src->m[1][0], src->m[1][1], src->m[1][3], src->m[2][0], src->m[2][1], src->m[2][3], src->m[3][0], src->m[3][1], src->m[3][3]);
    const float t03 = -det3x3(src->m[1][0], src->m[1][1], src->m[1][2], src->m[2][0], src->m[2][1], src->m[2][2], src->m[3][0], src->m[3][1], src->m[3][2]);

    const float t10 = -det3x3(src->m[0][1], src->m[0][2], src->m[0][3], src->m[2][1], src->m[2][2], src->m[2][3], src->m[3][1], src->m[3][2], src->m[3][3]);
    const float t11 =  det3x3(src->m[0][0], src->m[0][2], src->m[0][3], src->m[2][0], src->m[2][2], src->m[2][3], src->m[3][0], src->m[3][2], src->m[3][3]);
    const float t12 = -det3x3(src->m[0][0], src->m[0][1], src->m[0][3], src->m[2][0], src->m[2][1], src->m[2][3], src->m[3][0], src->m[3][1], src->m[3][3]);
    const float t13 =  det3x3(src->m[0][0], src->m[0][1], src->m[0][2], src->m[2][0], src->m[2][1], src->m[2][2], src->m[3][0], src->m[3][1], src->m[3][2]);

    const float t20 =  det3x3(src->m[0][1], src->m[0][2], src->m[0][3], src->m[1][1], src->m[1][2], src->m[1][3], src->m[3][1], src->m[3][2], src->m[3][3]);
    const float t21 = -det3x3(src->m[0][0], src->m[0][2], src->m[0][3], src->m[1][0], src->m[1][2], src->m[1][3], src->m[3][0], src->m[3][2], src->m[3][3]);
    const float t22 =  det3x3(src->m[0][0], src->m[0][1], src->m[0][3], src->m[1][0], src->m[1][1], src->m[1][3], src->m[3][0], src->m[3][1], src->m[3][3]);
    const float t23 = -det3x3(src->m[0][0], src->m[0][1], src->m[0][2], src->m[1][0], src->m[1][1], src->m[1][2], src->m[3][0], src->m[3][1], src->m[3][2]);

    const float t30 = -det3x3(src->m[0][1], src->m[0][2], src->m[0][3], src->m[1][1], src->m[1][2], src->m[1][3], src->m[2][1], src->m[2][2], src->m[2][3]);
    const float t31 =  det3x3(src->m[0][0], src->m[0][2], src->m[0][3], src->m[1][0], src->m[1][2], src->m[1][3], src->m[2][0], src->m[2][2], src->m[2][3]);
    const float t32 = -det3x3(src->m[0][0], src->m[0][1], src->m[0][3], src->m[1][0], src->m[1][1], src->m[1][3], src->m[2][0], src->m[2][1], src->m[2][3]);
    const float t33 =  det3x3(src->m[0][0], src->m[0][1], src->m[0][2], src->m[1][0], src->m[1][1], src->m[1][2], src->m[2][0], src->m[2][1], src->m[2][2]);

    dst->m[0][0] = t00 / det;
    dst->m[1][1] = t11 / det;
    dst->m[2][2] = t22 / det;
    dst->m[3][3] = t33 / det;

    dst->m[0][1] = t10 / det;
    dst->m[1][0] = t01 / det;
    dst->m[2][0] = t02 / det;
    dst->m[0][2] = t20 / det;

    dst->m[1][2] = t21 / det;
    dst->m[2][1] = t12 / det;
    dst->m[0][3] = t30 / det;
    dst->m[3][0] = t03 / det;

    dst->m[1][3] = t31 / det;
    dst->m[3][1] = t13 / det;
    dst->m[3][2] = t23 / det;
    dst->m[2][3] = t32 / det;

}








void barycentric(vec_t *dst, vec_t *A, vec_t *B, vec_t *C, vec_t *P) {
//    vec_t V1 = (vec_t){B->x-A->x, B->y-A->y, 0.0f, 0.0f};
//    vec_t V2 = (vec_t){C->x-A->x, C->y-A->y, 0.0f, 0.0f};
//    vec_t Q  = (vec_t){P->x-A->x, P->y-A->y, 0.0f, 0.0f};
//    const float c12 = vecCross2D(&V1, &V2);
//    dst->y = vecCross2D(&Q, &V2) / c12;
//    dst->z = vecCross2D(&V1, &Q) / c12;
//    dst->x = 1.0f - dst->y -  dst->z;

    const float v1x = B->x - A->x;
    const float v1y = B->y - A->y;

    const float v2x = C->x - A->x;
    const float v2y = C->y - A->y;

    const float qx = P->x - A->x;
    const float qy = P->y - A->y;

    const float c12 = v1x * v2y - v1y * v2x;

    dst->y = ( qx * v2y -  qy * v2x) / c12;
    dst->z = (v1x *  qy - v1y *  qx) / c12;
    dst->x = 1.0f - dst->y -  dst->z;

}


void baryCorrectPerspective(vec_t *bary, float w1, float w2, float w3, float oow, vec_t *dst) {
    static vec_t p1 = {1, 0, 0, 0};
    static vec_t p2 = {0, 1, 0, 0};
    static vec_t p3 = {0, 0, 1, 0};
    const float x = ( ((bary->x/w1)*p1.x) + ((bary->y/w2)*p2.x) + ((bary->z/w3)*p3.x) ) / oow;
    const float y = ( ((bary->x/w1)*p1.y) + ((bary->y/w2)*p2.y) + ((bary->z/w3)*p3.y) ) / oow;
    const float z = ( ((bary->x/w1)*p1.z) + ((bary->y/w2)*p2.z) + ((bary->z/w3)*p3.z) ) / oow;
    dst->x = x;
    dst->y = y;
    dst->z = z;
}



inline void interpolateBary(vec_t *dst, vec_t *A, vec_t *B, vec_t *C, vec_t *baryCoords) {
    const float bx = baryCoords->x;
    const float by = baryCoords->y;
    const float bz = baryCoords->z;
    dst->x = A->x * bx  +  B->x * by  +  C->x * bz;
    dst->y = A->y * bx  +  B->y * by  +  C->y * bz;
    dst->z = A->z * bx  +  B->z * by  +  C->z * bz;
    dst->w = A->w * bx  +  B->w * by  +  C->w * bz;
}



inline float reverseDepth(float z, float zNear, float zFar) {
    return (2.0f*zFar*zNear) / (zFar*(-z) + zFar + zNear*z + zNear);
}