#include "display.h"
#include "bitmap.h"
#include "objfile.h"
#include "model.h"
#include "renderer.h"
#include "camera.h"

#include <stdbool.h>
#include <stdio.h>


#define WIDTH 800
#define HEIGHT 600



bool w, a, s, d, q, e;
bool i, j, k, l, u, o;
int nKeysDown = 0;
model_t model;



void keyReleasedFunc(unsigned char key, int x, int y) {
    if(key == 'w') { w = false; nKeysDown--; }
    if(key == 'a') { a = false; nKeysDown--; }
    if(key == 's') { s = false; nKeysDown--; }
    if(key == 'd') { d = false; nKeysDown--; }
    if(key == 'q') { q = false; nKeysDown--; }
    if(key == 'e') { e = false; nKeysDown--; }

    if(key == 'i') { i = false; nKeysDown--; }
    if(key == 'j') { j = false; nKeysDown--; }
    if(key == 'k') { k = false; nKeysDown--; }
    if(key == 'l') { l = false; nKeysDown--; }
    if(key == 'u') { u = false; nKeysDown--; }
    if(key == 'o') { o = false; nKeysDown--; }
}




void keyPressedFunc(unsigned char key, int x, int y) {
    if(key == 'w') { w = true; nKeysDown++; }
    if(key == 'a') { a = true; nKeysDown++; }
    if(key == 's') { s = true; nKeysDown++; }
    if(key == 'd') { d = true; nKeysDown++; }
    if(key == 'q') { q = true; nKeysDown++; }
    if(key == 'e') { e = true; nKeysDown++; }

    if(key == 'i') { i = true; nKeysDown++; }
    if(key == 'j') { j = true; nKeysDown++; }
    if(key == 'k') { k = true; nKeysDown++; }
    if(key == 'l') { l = true; nKeysDown++; }
    if(key == 'u') { u = true; nKeysDown++; }
    if(key == 'o') { o = true; nKeysDown++; }
}




void create() {
    obj_model_t objmodel;
    objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head.obj", &objmodel);
    mdlCreateFromObj(&objmodel, &model);
    objFree(&objmodel);

    srInit(WIDTH, HEIGHT);

}




void updateFunc(bitmap_t *displayBuffer) {

    double camSpeed = 0.4 ;
    if(w) { camMove(&srCamera, 0,  camSpeed); }
    if(s) { camMove(&srCamera, 0, -camSpeed); }
    if(a) { camMove(&srCamera, 1,  camSpeed); }
    if(d) { camMove(&srCamera, 1, -camSpeed); }
    if(q) { camMove(&srCamera, 2,  camSpeed); }
    if(e) { camMove(&srCamera, 2, -camSpeed); }

    srRender(displayBuffer, &model);

    if(nKeysDown == 0) {
        if(dpIsUsingLowRes() == 1) {
            dpUseFullRes();
        }
    } else {
        if(dpIsUsingLowRes() == 0) {
            dpUseLowRes();
        }
    }
}




void exitFunc() {
    dpDispose();
}




int main(int argc, char *argv[]) {
    dpCreate(argc, argv, WIDTH, HEIGHT, 60);

    dpSetUpdateFunc(&updateFunc);
    dpSetExitFunc(&exitFunc);
    dpSetKeyPressedFunc(&keyPressedFunc);
    dpSetKeyReleasedFunc(&keyReleasedFunc);

    create();

    dpStart();

    return 0;
}