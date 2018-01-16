#include "camera.h"


void camUpdate(camera_t *cam) {
    matSetLookAt(&cam->view, &cam->pos, &cam->target, &cam->up);
    matMul(&cam->viewProjection, &cam->projection, &cam->view);
}




// dir: 0 = forward; 1 = right; 2 = up
void camMove(camera_t *cam, int dir, double amt) {

    vec_t up = {0, 1, 0};
    vec_t forward, right;

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
