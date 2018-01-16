#ifndef SOFTWARERENDERER_RENDERER_H
#define SOFTWARERENDERER_RENDERER_H

#include "model.h"
#include "bitmap.h"
#include "camera.h"


camera_t srCamera;


void srInit(float width, float height);

void srRenderWireframe(bitmap_t *bitmap, model_t *model);


#endif
