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

float *posArrayFull;
float *posArrayLow;


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

    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    watchStart("prepOpenGL");

    if(useLowResBuffer == 1) {
        glPointSize(lowResScale);
        glVertexPointer(2, GL_FLOAT, 0, posArrayLow);
        glColorPointer(3, GL_FLOAT, sizeof(pixel_t), &(displayBufferCurrent->pixels->color.r));
        glDrawArrays(GL_POINTS, 0, displayBufferLow.width*displayBufferLow.height);

    } else {
        glPointSize(1);
        glVertexPointer(2, GL_FLOAT, 0, posArrayFull);
        glColorPointer(3, GL_FLOAT, sizeof(pixel_t), &(displayBufferCurrent->pixels->color.r));
        glDrawArrays(GL_POINTS, 0, displayBufferFull.width*displayBufferFull.height);
    }

    watchEnd("prepOpenGL");


    glutSwapBuffers();

    watchEnd("openGL");

    //watchPrintData();

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
    bmCreate(&displayBufferLow, (unsigned int)(width/lowResScale), (unsigned int)(height/lowResScale));
    displayBufferCurrent = &displayBufferFull;
    posArrayFull = calloc(displayBufferFull.width*displayBufferFull.height*2, sizeof(float));
    posArrayLow = calloc(displayBufferLow.width*displayBufferLow.height*2, sizeof(float));

    int iF=0;
    for(int y=0; y<displayBufferFull.height; y++) {
        for(int x=0; x<displayBufferFull.width; x++) {

            float glX = toGLCoords(x, displayBufferFull.width,  false);
            float glY = toGLCoords(y, displayBufferFull.height, true);

            posArrayFull[iF++] = glX;
            posArrayFull[iF++] = glY;
        }
    }

    int iL=0;
    for(int y=0; y<displayBufferLow.height; y++) {
        for(int x=0; x<displayBufferLow.width; x++) {
            float glX = toGLCoords(x, displayBufferLow.width,  false);
            float glY = toGLCoords(y, displayBufferLow.height, true);
            posArrayLow[iL++] = glX;
            posArrayLow[iL++] = glY;
        }
    }

}




void dpDispose() {
    bmDispose(&displayBufferFull);
    bmDispose(&displayBufferLow);
    free(posArrayFull);
    free(posArrayLow);
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
