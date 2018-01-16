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
model_t model;



void keyReleasedFunc(unsigned char key, int x, int y) {
    if(key == 'w') { w = false; }
    if(key == 'a') { a = false; }
    if(key == 's') { s = false; }
    if(key == 'd') { d = false; }
    if(key == 'q') { q = false; }
    if(key == 'e') { e = false; }

    if(key == 'i') { i = false; }
    if(key == 'j') { j = false; }
    if(key == 'k') { k = false; }
    if(key == 'l') { l = false; }
    if(key == 'u') { u = false; }
    if(key == 'o') { o = false; }
}




void keyPressedFunc(unsigned char key, int x, int y) {
    if(key == 'w') { w = true; }
    if(key == 'a') { a = true; }
    if(key == 's') { s = true; }
    if(key == 'd') { d = true; }
    if(key == 'q') { q = true; }
    if(key == 'e') { e = true; }

    if(key == 'i') { i = true; }
    if(key == 'j') { j = true; }
    if(key == 'k') { k = true; }
    if(key == 'l') { l = true; }
    if(key == 'u') { u = true; }
    if(key == 'o') { o = true; }
}




void create() {
    obj_model_t objmodel;
    objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head.obj", &objmodel);
    mdlCreateFromObj(&objmodel, &model);
    objFree(&objmodel);

    srInit(WIDTH, HEIGHT);

}




void updateFunc(bitmap_t *displayBuffer) {

    double camSpeed = 0.3 ;
    if(w) { camMove(&srCamera, 0,  camSpeed); }
    if(s) { camMove(&srCamera, 0, -camSpeed); }
    if(a) { camMove(&srCamera, 1, -camSpeed); }
    if(d) { camMove(&srCamera, 1,  camSpeed); }
    if(q) { camMove(&srCamera, 2, -camSpeed); }
    if(e) { camMove(&srCamera, 2,  camSpeed); }

    srRenderWireframe(displayBuffer, &model);
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