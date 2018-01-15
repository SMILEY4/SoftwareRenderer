#ifndef SOFTWARERENDERER_BRESENHAM_H
#define SOFTWARERENDERER_BRESENHAM_H

#include "bitmap.h"

void bhDrawLine(bitmap_t *bitmap, int ax, int ay, int bx, int by, float r, float g, float b);

void bhDrawTriangle(bitmap_t *bitmap, int ax, int ay, int bx, int by, int cx, int cy);


#endif