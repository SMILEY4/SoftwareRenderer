#include "display.h"
#include "input.h"
#include "rendercontext.h"
#include "objfile.h"
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 600


camera_t camera;
model_t model;


void create() {

    // MODEL
    obj_model_t obj_diablo;
    objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\smoothMonkeyU.obj", &obj_diablo);
//    objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose.obj", &obj_diablo);
    char *texturesDiablo[5] = {
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_diffuse.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_nm.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_nm_tangent.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_spec.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_glow.png"
    };
    mdlCreateFromObj(&obj_diablo, &model, texturesDiablo, 5, 1);
    objFree(&obj_diablo);

    model.translation = (vec_t){ 0,   0,  0, 0};
    model.rotation =    (vec_t){ 0,   1,  0, 0};
    model.scale =       (vec_t){ 10, 10, 10, 0};
    mdlUpdateTransform(&model);


    // CAMERA
    vec_t camPos = (vec_t){-22.481f, 0.0f, 3.6902f, 1.0f};
    vec_t camTgt = (vec_t){0, 0, 0, 1};
    vec_t camUp = (vec_t){0, 1, 0, 1};
    camCreateEXT(&camera, WIDTH, HEIGHT, 70.0, 0.1f, 100.0f, camPos, camTgt, camUp);


}




void updateFunc(bitmap_t *displayBuffer) {

    double camSpeed = 0.4 ;
    if(inGetKeyState('w') == IN_DOWN) { camMove(&camera, 0,  camSpeed); }
    if(inGetKeyState('s') == IN_DOWN) { camMove(&camera, 0, -camSpeed); }
    if(inGetKeyState('a') == IN_DOWN) { camMove(&camera, 1,  camSpeed); }
    if(inGetKeyState('d') == IN_DOWN) { camMove(&camera, 1, -camSpeed); }
    if(inGetKeyState('q') == IN_DOWN) { camMove(&camera, 2,  camSpeed); }
    if(inGetKeyState('e') == IN_DOWN) { camMove(&camera, 2, -camSpeed); }
    camSetRendertargetEXT(&camera, displayBuffer, 1);

    rcDrawModelWireframe(&camera, &model);
}




void exitFunc() {
    dpDispose();
}




int main(int argc, char *argv[]) {

    dpCreate(argc, argv, WIDTH, HEIGHT, 60, 2.0f);

    dpSetUpdateFunc(&updateFunc);
    dpSetExitFunc(&exitFunc);
    create();

    dpStart();

    return 0;
}