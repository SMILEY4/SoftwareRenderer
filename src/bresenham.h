#ifndef SOFTWARERENDERER_BRESENHAM_H
#define SOFTWARERENDERER_BRESENHAM_H

#include "bitmap.h"

void bhDrawLine(bitmap_t *bitmap, int ax, int ay, int bx, int by, color_t color);

void bhDrawTriangle(bitmap_t *bitmap, int ax, int ay, int bx, int by, int cx, int cy, color_t ca, color_t cb, color_t cc);


#endif