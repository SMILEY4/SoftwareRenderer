#include "display.h"
#include "bitmap.h"
#include "objfile.h"
#include "model.h"
#include "renderer.h"
#include "camera.h"
#include "stopwatch.h"
#include "bresenham.h"
#include "geometry.h"
#include "shader.h"
#include "lodepng.h"

#include "postProcess.h"
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <time.h>
#include <stdbool.h>

#define toRadians(angleDegrees) ((angleDegrees) * M_PI / 180.0)
#define toDegrees(angleRadians) ((angleRadians) * 180.0 / M_PI)

#define WIDTH 800
#define HEIGHT 600


// input
bool w, a, s, d, q, e;
bool i, j, k, l, u, o;
bool space;
int nKeysDown = 0;


// models
model_t modelHead;
model_t modelDiablo;
model_t modelPlane;

// cameras
camera_t cameraMain;
camera_t camLight;

// renderdata
renderdata_t dataShadow;
renderdata_t dataMain;

// misc
vec_t lightpos;
bool renderShadowmap;
bitmap_t skybox;
matrix_t biasMatrix;
matrix_t depthBiasMVP;






void create() {

    // CREATE MODELS
    obj_model_t obj_plane;
    objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane.obj", &obj_plane);
    char *texturesPlane[5] = {
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head\\african_head_diffuse.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head\\african_head_nm.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head\\african_head_nm_tangent.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head\\african_head_spec.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head\\african_head_SSS.png"
    };
    mdlCreateFromObj(&obj_plane, &modelPlane, texturesPlane, 5, 1);
    objFree(&obj_plane);

    modelPlane.translation = (vec_t){ 0,   0,  0, 0};
    modelPlane.rotation =    (vec_t){ 0,   0,  0, 0};
    modelPlane.scale =       (vec_t){ 10,  1, 10, 0};
    mdlUpdateTransform(&modelPlane);

//
//    obj_model_t obj_head;
//    objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head\\african_head.obj", &obj_head);
//    char *texturesHead[5] = {
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head\\african_head_diffuse.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head\\african_head_nm.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head\\african_head_nm_tangent.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head\\african_head_spec.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head\\african_head_SSS.png"
//    };
//    mdlCreateFromObj(&obj_head, &modelHead, texturesHead, 5, 1);
//    objFree(&obj_head);
//
//    modelHead.translation = (vec_t){ 0,   0,  0, 0};
//    modelHead.rotation =    (vec_t){ 0,   1,  0, 0};
//    modelHead.scale =       (vec_t){ 7,  -7,  7, 0};
//    mdlUpdateTransform(&modelHead);
//
//
//    obj_model_t ob_diablo;
//    objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose.obj", &ob_diablo);
//    char *texturesDiablo[5] = {
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_diffuse.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_nm.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_nm_tangent.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_spec.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_glow.png"
//    };
//    mdlCreateFromObj(&ob_diablo, &modelDiablo, texturesDiablo, 5, 1);
//    objFree(&ob_diablo);
//
//    modelDiablo.translation = (vec_t){ 0,   0,  0, 0};
//    modelDiablo.rotation =    (vec_t){ 0,   1,  0, 0};
//    modelDiablo.scale =       (vec_t){ 10,-10, 10, 0};
//    mdlUpdateTransform(&modelDiablo);


    // SETUP SKYBOX
    bmCreateFromPNG(&skybox, "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\Shiodome_Stairs.png");


    // CAMERA
    vec_t camPos = (vec_t){-22.481f, 0.0f, 3.6902f, 1.0f};
    vec_t camTgt = (vec_t){0, 0, 0, 1};
    vec_t camUp = (vec_t){0, 1, 0, 1};
    camCreateEXT(&cameraMain, WIDTH, HEIGHT, 70.0, 0.1f, 100.0f, camPos, camTgt, camUp);


    // LIGHT
    lightpos = (vec_t){-15.775871f, -7.742854f, -2.031374f, 1.0000f};
    camCreateFS(&camLight, 800, 800, 1, 70, 0.1f, 100.0f, lightpos, camTgt, camUp);


    // MISC
    renderShadowmap = true;
    biasMatrix = (matrix_t) {
            0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f
    };


    // RENDER DATA

    // shadow pass
    dataShadow.nModels = 1;
    dataShadow.models = &modelDiablo;
    dataShadow.camera = &camLight;
    dataShadow.cullingMode = 2;
    dataShadow.vsh = &shaderVertex_shadow;
    dataShadow.fsh = &shaderFragment_shadow;
    dataShadow.nUniformVars = 0;


    // main pass
    dataMain.nModels = 1;
    dataMain.models = &modelDiablo;
    dataMain.camera = &cameraMain;
    dataMain.cullingMode = 1;
    dataMain.vsh = &shaderVertex_main;
    dataMain.fsh = &shaderFragment_main;
    dataMain.nUniformVars = 4;
    dataMain.uniformVars = calloc((size_t)dataMain.nUniformVars, sizeof(matrix_t));
    dataMain.uniformVars[0] = &dataShadow.camera->rendertargets[0];
    dataMain.uniformVars[1] = &skybox;
    dataMain.uniformVars[2] = &camLight;
    dataMain.uniformVars[3] = &depthMVP;

}




void updateFunc(bitmap_t *displayBuffer) {

    // HANDLE INPUT
    double camSpeed = 0.4 ;
    if(w) { camMove(&cameraMain, 0,  camSpeed); }
    if(s) { camMove(&cameraMain, 0, -camSpeed); }
    if(a) { camMove(&cameraMain, 1,  camSpeed); }
    if(d) { camMove(&cameraMain, 1, -camSpeed); }
    if(q) { camMove(&cameraMain, 2,  camSpeed); }
    if(e) { camMove(&cameraMain, 2, -camSpeed); }

    // UPDATE CAMERA
    camSetRendertargetEXT(&cameraMain, displayBuffer, 1);


    // DRAW - SHADOW PASS
    if(renderShadowmap) {
        matMul(&depthBiasMVP, &biasMatrix, &depthMVP);
        bmClear(&camLight.rendertargets[0], &(color_t){0.1f, 0.1f, 0.1f, 0.0f});
        render(&dataShadow);
        renderShadowmap = false;
    }

    // DRAW - MAIN PASS
    render(&dataMain);

    // DRAW - PP-PASS
    //ppAmbientOcclusion(displayBuffer);


    // SWITCH RESOLUTION
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

    if(key == 32) { space = false; nKeysDown--; }
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

    if(key == 32) { space = true; nKeysDown++; }

    if(key == 32) {
        //camera.pos = (vec_t){lightpos.x, lightpos.y, lightpos.z, 1.0f};
        camUpdate(&cameraMain);
    }

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

        printf("CAM POS: %f %f %f \n", cameraMain.pos.x, cameraMain.pos.y, cameraMain.pos.z);
        printf("==============\n");

    }

}







void exitFunc() {
    bmDispose(&skybox);
    dpDispose();
    watchFreeData();
    mdlFreeModel(&modelHead);
    mdlFreeModel(&modelDiablo);
    free(dataMain.uniformVars);
    free(dataShadow.uniformVars);
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