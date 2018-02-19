#include "display.h"
#include "bitmap.h"
#include "objfile.h"
#include "model.h"
#include "renderer.h"
#include "camera.h"
#include "stopwatch.h"
#include "bresenham.h"
#include "geometry.h"

#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>

#define toRadians(angleDegrees) ((angleDegrees) * M_PI / 180.0)
#define toDegrees(angleRadians) ((angleRadians) * 180.0 / M_PI

#define WIDTH 800
#define HEIGHT 600



bool w, a, s, d, q, e;
bool i, j, k, l, u, o;
int nKeysDown = 0;

model_t model;
camera_t camera;


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


    if(key == 'i') {
        pixel_t *pixel = bmGetPixelAt(dpGetBuffer(), x, y);
        if(pixel) {

            printf("==============\n");
            printf("PIXEL AT %d %d\n", x, y);
            printf("Color:\n");
            printf("  r:%f\n", pixel->color.r);
            printf("  g:%f\n", pixel->color.g);
            printf("  b:%f\n", pixel->color.b);
            printf("  a:%f\n", pixel->color.a);
            printf("depth:%f\n", pixel->depth);
            printf("==============\n");

        }
    }

}




void create() {

    // LOAD MODEL
    obj_model_t objmodel;
    objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head.obj", &objmodel);
    mdlCreateFromObj(&objmodel, &model, "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head_diffuse.png", 0);
    //objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo3_pose.obj", &objmodel);
    //mdlCreateFromObj(&objmodel, &model, "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo3_pose_diffuse.png");
    objFree(&objmodel);


    // SETUP MODEL
    model.translation = (vec_t){ 0,   0,  0, 0};
    model.rotation =    (vec_t){ 0,   1,  0, 0};
    model.scale =       (vec_t){10, -10, 10, 0};
    mdlUpdateTransform(&model);


    // CAMERA
    vec_t camPos = (vec_t){-22.481f, 0.0f, 3.6902f, 1.0f};
    vec_t camTgt = (vec_t){0, 0, 0, 1};
    vec_t camUp = (vec_t){0, 1, 0, 1};
    camCreateEXT(&camera, WIDTH, HEIGHT, 70.0, 0.1f, 1000.0f, camPos, camTgt, camUp);

}




void updateFunc(bitmap_t *displayBuffer) {

    // HANDLE INPUT
    double camSpeed = 0.4 ;
    if(w) { camMove(&camera, 0,  camSpeed); }
    if(s) { camMove(&camera, 0, -camSpeed); }
    if(a) { camMove(&camera, 1,  camSpeed); }
    if(d) { camMove(&camera, 1, -camSpeed); }
    if(q) { camMove(&camera, 2,  camSpeed); }
    if(e) { camMove(&camera, 2, -camSpeed); }

    double mdlSpeed = 0.1 ;
    if(l) { model.rotation.y += mdlSpeed;  mdlUpdateTransform(&model); }
    if(j) { model.rotation.y -= mdlSpeed;  mdlUpdateTransform(&model); }


    // UPDATE CAMERA
    camSetRendertargetEXT(&camera, displayBuffer, 1);


    // DRAW
    static matrix_t modelViewProjection;
    matMul(&modelViewProjection, &camera.viewProjection, &model.modelTransform);

    // set render data
    renderdata_t data;
    data.model = &model;
    data.camera = &camera;
    data.nUniformVars = 2;
    data.uniformVars = calloc((size_t)data.nUniformVars, sizeof(matrix_t));
    data.uniformVars[0] = &modelViewProjection;
    data.uniformVars[1] = &model.modelTransform;

    // render
    render(&data);


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
    watchFreeData();
    mdlFreeModel(&model);
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