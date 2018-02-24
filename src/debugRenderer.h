#ifndef SOFTWARERENDERER_DEBUGRENDERER_H
#define SOFTWARERENDERER_DEBUGRENDERER_H


#include "bitmap.h"
#include "renderer.h"




void dbDrawLine(bitmap_t *target, int x0, int y0, int x1, int y1, color_t *color);

void dbDrawLine3D(bitmap_t *target, int x0, int y0, int z0, int x1, int y1, int z1, color_t *color, camera_t *camera);

void dbDrawDisk(bitmap_t *target, int x, int y, int radius, color_t *color);

void dbDrawDisk3D(bitmap_t *target, int x, int y, int z, int radius, color_t *color, camera_t *camera);


#endif
