#ifndef SOFTWARERENDERER_DISPLAY_H
#define SOFTWARERENDERER_DISPLAY_H


#include "bitmap.h"


void dpStart();

void dpCreate(int argc, char *argv[], unsigned int width, unsigned int height, float fps);

void dpDispose();

void dpSetUpdateFunc( void (*func)(bitmap_t *bitmap) );

void dpSetExitFunc( void (*f)(void) );

void dpSetKeyPressedFunc( void (*f)(unsigned char key, int x, int y) );

void dpSetKeyReleasedFunc( void (*f)(unsigned char key, int x, int y) );


#endif
