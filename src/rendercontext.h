#ifndef SOFTWARERENDERERV2_RENDERCONTEXT_H
#define SOFTWARERENDERERV2_RENDERCONTEXT_H

#include "camera.h"
#include "model.h"
#include "shader.h"


int g_pickedTriangle;

void rcDrawModel(camera_t *camera, model_t *model, shader_t *shader);

#endif
