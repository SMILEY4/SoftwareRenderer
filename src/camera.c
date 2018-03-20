#include "camera.h"
#include "geometry.h"
#include "bitmap.h"
#include <stdio.h>
#include <math.h>
#include <windows.h>




// fixed size
void camCreateFS(camera_t *camera, float width, float height, unsigned int nRenderTargets, float fovDeg, float zNear, float zFar, vec_t pos, vec_t target, vec_t up) {

    camera->aspectRatio = width / height;
    camera->fovDeg = fovDeg;
    camera->zNear = zNear;
    camera->zFar = zFar;
    camera->pos = pos;
    camera->target = target;
    camera->up = up;
    camUpdate(camera);

    if(nRenderTargets > 0) {
        camera->nRenderTargets = nRenderTargets;
        camera->rendertargets = calloc(nRenderTargets, sizeof(bitmap_t));
        for(int i=0; i<nRenderTargets; i++) {
            bitmap_t rt;
            bmCreate(&rt, (unsigned int)width, (unsigned int)height);
            camera->rendertargets[i] = rt;
        }
    }
    matSetScreenSpaceTransform(&camera->screenSpaceTransform, width/2.0f, height/2.0f);
}




// custom render targets (rendertargets[0] = mainRT)
void camCreateRT(camera_t *camera, float width, float height, bitmap_t *rendertargets, unsigned int nRenderTargets, float fovDeg, float zNear, float zFar, vec_t pos, vec_t target, vec_t up) {

    camera->aspectRatio = width / height;
    camera->fovDeg = fovDeg;
    camera->zNear = zNear;
    camera->zFar = zFar;
    camera->pos = pos;
    camera->target = target;
    camera->up = up;
    camUpdate(camera);


    camera->nRenderTargets = nRenderTargets;
    camera->rendertargets = rendertargets;

    bitmap_t mainRT = camera->rendertargets[0];
    matSetScreenSpaceTransform(&camera->screenSpaceTransform, mainRT.width/2.0f, mainRT.height/2.0f);

}


// external render targets (rendertargets[0] = mainRT)
void camCreateEXT(camera_t *camera, float width, float height, float fovDeg, float zNear, float zFar, vec_t pos, vec_t target, vec_t up) {
    camera->aspectRatio = width / height;
    camera->fovDeg = fovDeg;
    camera->zNear = zNear;
    camera->zFar = zFar;
    camera->pos = pos;
    camera->target = target;
    camera->up = up;
    camUpdate(camera);
}




void camSetRendertargetEXT(camera_t *camera, bitmap_t *rendertargets, unsigned int nRendertargets) {
    camera->nRenderTargets = nRendertargets;
    camera->rendertargets = rendertargets;
    matSetScreenSpaceTransform(&camera->screenSpaceTransform, rendertargets[0].width/2.0f, rendertargets[0].height/2.0f);
}





void camUpdate(camera_t *cam) {
    matSetPerspective(&cam->projection, (float)((cam->fovDeg) * M_PI / 180.0), cam->aspectRatio, cam->zNear, cam->zFar);
    matSetLookAt(&cam->view, &cam->pos, &cam->target, &cam->up);
    matMul(&cam->viewProjection, &cam->projection, &cam->view);
}




// dir:   0 = forward;   1 = right;   2 = up
void camMove(camera_t *cam, int dir, double amt) {

    vec_t up = {0, 1, 0, 0};
    vec_t forward = {0, 0, 0, 0};
    vec_t right = {0, 0, 0, 0};

    vecSub(&forward, &cam->target, &cam->pos);
    vecNormalize(&forward, &forward);

    vecCross(&right, &forward, &cam->up);
    vecNormalize(&right, &right);

    if(dir == 0) {
        forward.x *= amt;
        forward.y *= amt;
        forward.z *= amt;
        vecAdd(&cam->pos, &cam->pos, &forward);
    }
    if(dir == 1) {
        right.x *= amt;
        right.y *= amt;
        right.z *= amt;
        vecAdd(&cam->pos, &cam->pos, &right);
    }
    if(dir == 2) {
        up.x *= amt;
        up.y *= amt;
        up.z *= amt;
        vecAdd(&cam->pos, &cam->pos, &up);
    }

    camUpdate(cam);
}


void camDispose(camera_t *camera) {
    for(int i=0; i<camera->nRenderTargets; i++) {
        bmDispose(camera->rendertargets+i);
    }
    free(camera->rendertargets);
}


