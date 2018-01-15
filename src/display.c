#include "bitmap.h"
#include "display.h"
#include <windows.h>
#include <GL/glut.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>




float deltaTime;
bitmap_t displayBuffer;
color_t clearColor;

void (*updateFunction)(bitmap_t *bitmap);




void timerFunc(int value) {
    glutPostRedisplay();
}




float toGLCoords(float x, float size, bool flip) {
    float a = x / size;
    return (float)((a*2.0) - 1.0) * (flip ? -1 : +1);
}




void displayFunc() {

    clock_t start, end;
    start = clock();

    bmClear(&displayBuffer, &clearColor);
    (*updateFunction)(&displayBuffer);

    glClear(GL_COLOR_BUFFER_BIT);
    glPointSize(1);
    glBegin(GL_POINTS);

    for(int y=0; y<displayBuffer.height; y++) {
        for(int x=0; x<displayBuffer.width; x++) {
            pixel_t *pixel = bmGetPixelAt(&displayBuffer, x, y);
            color_t color = pixel->color;
            if(color.a > 0) {
                glColor4f(color.r, color.g, color.b, color.a);
                glVertex2f(toGLCoords(x, displayBuffer.width, false), toGLCoords(y, displayBuffer.height, true));
            }
        }
    }

    glEnd();
    glutSwapBuffers();

    end = clock();
    float dtms = (float)( ((end - start) / CLOCKS_PER_SEC) * 1000.0 );
    float sleep = fmaxf(1.0, deltaTime-dtms);
    glutTimerFunc((unsigned int)sleep, timerFunc, 1);
}




void dpStart() {
    glutTimerFunc(0, timerFunc, 1);
    glutMainLoop();
}




void dpCreate(int argc, char *argv[], unsigned int width, unsigned int height, float fps) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutCreateWindow("Software Renderer - Lukas Ruegner (2018)");
    glutDisplayFunc(displayFunc);
    glutIgnoreKeyRepeat(1);
    deltaTime = (float)(1000.0 / fps);
    bmCreate(&displayBuffer, width, height);
    clearColor = (color_t){0, 0, 0, 0};
}




void dpDispose() {
    bmDispose(&displayBuffer);
}




void dpSetUpdateFunc( void (*f)(bitmap_t *bitmap) ) {
    updateFunction = f;
}




void dpSetExitFunc( void (*f)(void) ) {
    atexit(f);
}




void dpSetKeyPressedFunc( void (*f)(unsigned char key, int x, int y) ) {
    glutKeyboardFunc(f);
}




void dpSetKeyReleasedFunc( void (*f)(unsigned char key, int x, int y) ) {
    glutKeyboardUpFunc(f);
}
