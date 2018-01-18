#include "bitmap.h"
#include "display.h"
#include "stopwatch.h"
#include <windows.h>
#include <GL/glut.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>



char *title;

float deltaTime;
color_t clearColor;

bitmap_t displayBufferFull;
bitmap_t displayBufferLow;
bitmap_t *displayBufferCurrent;
float lowResScale = 4.0;
int useLowResBuffer = 0;

void (*updateFunction)(bitmap_t *bitmap);




void timerFunc(int value) {
    glutPostRedisplay();
}




float toGLCoords(float x, float size, bool flip) {
    float a = x / size;
    return (float)((a*2.0) - 1.0) * (flip ? -1 : +1);
}




void displayFunc() {

    //watchReset();

    clock_t start, end;
    start = clock();

    bmClear(displayBufferCurrent, &clearColor);
    (*updateFunction)(displayBufferCurrent);

    watchStart("openGL");

    glClear(GL_COLOR_BUFFER_BIT);
    glPointSize( (useLowResBuffer ? lowResScale : 1) );
    glBegin(GL_POINTS);

    for(int y=0; y<displayBufferCurrent->height; y++) {
        for(int x=0; x<displayBufferCurrent->width; x++) {
            pixel_t *pixel = bmGetPixelAt(displayBufferCurrent, x, y);
            color_t color = pixel->color;
            if(color.a > 0) {
                glColor4f(color.r, color.g, color.b, color.a);
                glVertex2f(toGLCoords(x, displayBufferCurrent->width, false), toGLCoords(y, displayBufferCurrent->height, true));
            }
        }
    }

    glEnd();
    glutSwapBuffers();

    watchEnd("openGL");

   // watchPrintData();

    end = clock();
    float dtms = (float)(end - start);
    float sleep = fmaxf(1.0, deltaTime-dtms);
    glutTimerFunc((unsigned int)sleep, timerFunc, 1);
}




void dpStart() {
    glutTimerFunc(0, timerFunc, 1);
    glutMainLoop();
}




void dpCreate(int argc, char *argv[], unsigned int width, unsigned int height, float fps) {
    title = "Software Renderer - Lukas Ruegner (2018)";
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutCreateWindow(title);
    glutDisplayFunc(displayFunc);
    glutIgnoreKeyRepeat(1);
    deltaTime = (float)(1000.0 / fps);
    clearColor = (color_t){0, 0, 0, 0};
    bmCreate(&displayBufferFull, width, height);
    bmCreate(&displayBufferLow, width/lowResScale, height/lowResScale);
    displayBufferCurrent = &displayBufferFull;
}




void dpDispose() {
    bmDispose(&displayBufferFull);
    bmDispose(&displayBufferLow);
}




void dpSetUpdateFunc( void (*f)(bitmap_t *bitmap) ) {
    updateFunction = f;
}




void dpSetExitFunc( void (*f)(void) ) {
    atexit(f);
}




void dpUseLowRes() {
    bmClear(displayBufferCurrent, &clearColor);
    displayBufferCurrent = &displayBufferLow;
    useLowResBuffer = 1;
}




void dpUseFullRes() {
    bmClear(displayBufferCurrent, &clearColor);
    displayBufferCurrent = &displayBufferFull;
    useLowResBuffer = 0;
}




int dpIsUsingLowRes() {
    return useLowResBuffer;
}




bitmap_t *dpGetBuffer() {
    return &displayBufferFull;
}




void dpSetKeyPressedFunc( void (*f)(unsigned char key, int x, int y) ) {
    glutKeyboardFunc(f);
}




void dpSetKeyReleasedFunc( void (*f)(unsigned char key, int x, int y) ) {
    glutKeyboardUpFunc(f);
}
