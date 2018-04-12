#ifndef SOFTWARERENDERER_BRESENHAM_H
#define SOFTWARERENDERER_BRESENHAM_H

#include "bitmap.h"

void bhDrawLineToScanbuffer(int *scanbufferMin, int *scanbufferMax, int scanbufferSize, int ax, int ay, int bx, int by);

void bhDrawLine(bitmap_t *bitmap, int ax, int ay, int bx, int by, float r, float g, float b);

void bhDrawLine3D(bitmap_t *bitmap, int ax, int ay, float az, int bx, int by, float bz, float r, float g, float b);


#endif