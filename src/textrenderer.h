
#ifndef SOFTWARERENDERERV2_TEXTRENDERER_H
#define SOFTWARERENDERERV2_TEXTRENDERER_H

#include "bitmap.h"


void trCreateFont();

void trDrawString(bitmap_t *bitmap, const char *str, int len, int x, int y, int scale);

#endif
