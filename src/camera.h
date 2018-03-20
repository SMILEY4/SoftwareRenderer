#ifndef SOFTWARERENDERER_CAMERA_H
#define SOFTWARERENDERER_CAMERA_H


#include "geometry.h"
#include "bitmap.h"


typedef struct {
    float aspectRatio;
    float fovDeg;
    float zNear;
    float zFar;

    bitmap_t *rendertargets;
    unsigned int nRenderTargets;

    vec_t pos;
    vec_t target;
    vec_t up;

    matrix_t view;
    matrix_t projection;
    matrix_t viewProjection;
    matrix_t screenSpaceTransform;

} camera_t;


void camCreateFS(camera_t *camera, float width, float height, unsigned int nRenderTargets, float fovDeg, float zNear, float zFar, vec_t pos, vec_t target, vec_t up);

void camCreateRT(camera_t *camera, float width, float height, bitmap_t *rendertargets, unsigned int nRenderTargets, float fovDeg, float zNear, float zFar, vec_t pos, vec_t target, vec_t up);

void camCreateEXT(camera_t *camera, float width, float height, float fovDeg, float zNear, float zFar, vec_t pos, vec_t target, vec_t up);

void camSetRendertargetEXT(camera_t *camera, bitmap_t *rendertargets, unsigned int nRendertargets);

void camUpdate(camera_t *cam);

void camMove(camera_t *cam, int dir, double amt);

void camDispose(camera_t *camera);


#endif
