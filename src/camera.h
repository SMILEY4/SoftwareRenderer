#ifndef SOFTWARERENDERER_CAMERA_H
#define SOFTWARERENDERER_CAMERA_H


#include "geometry.h"


typedef struct {
    vec_t pos;
    vec_t target;
    vec_t up;
    matrix_t view;
    matrix_t projection;
    matrix_t viewProjection;
    matrix_t screenSpaceTransform;
} camera_t;




void camUpdate(camera_t *cam);

void camMove(camera_t *cam, int dir, double amt);


#endif
