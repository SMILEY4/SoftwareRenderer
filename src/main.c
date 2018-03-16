#include "display.h"
#include "input.h"
#include "rendercontext.h"
#include "objfile.h"
#include "camera.h"
#include "bitmap.h"
#include "uniforms.h"
#include "shader.h"
#include "model.h"
#include <stdio.h>
#include <windows.h>

#define WIDTH 800
#define HEIGHT 600


camera_t screenshotCam;
camera_t camera;
model_t model;
shader_t shader;





void create() {

    // MODEL
    obj_model_t obj_diablo;
  objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane.obj", &obj_diablo);
//  objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\smoothMonkeyU.obj", &obj_diablo);
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

    model.translation = (vec_t){ 0,   -5,  0, 0};
    model.rotation =    (vec_t){ 0,   0,  0, 0};
    model.scale =       (vec_t){ 10, 10, 10, 0};
    mdlUpdateTransform(&model);


    // CAMERA
    vec_t camPos = (vec_t){-22.481f, 0.0f, 3.6902f, 1.0f};
    vec_t camTgt = (vec_t){0, 0, 0, 1};
    vec_t camUp = (vec_t){0, 1, 0, 1};
    camCreateEXT(&camera, WIDTH, HEIGHT, 70.0, 0.1f, 100.0f, camPos, camTgt, camUp);


    // SHADER
    shader.vsh = vshDefault;
    shader.fsh = fshDefault;
    ubCreateBuffer(&shader.uniforms, 8);

}




void updateFunc(bitmap_t *displayBuffer) {

    // UPDATE CAMERA
    double camSpeed = 0.4 ;
    if(inGetKeyState('w') == IN_DOWN) { camMove(&camera, 0,  camSpeed); }
    if(inGetKeyState('s') == IN_DOWN) { camMove(&camera, 0, -camSpeed); }
    if(inGetKeyState('a') == IN_DOWN) { camMove(&camera, 1,  camSpeed); }
    if(inGetKeyState('d') == IN_DOWN) { camMove(&camera, 1, -camSpeed); }
    if(inGetKeyState('q') == IN_DOWN) { camMove(&camera, 2,  camSpeed); }
    if(inGetKeyState('e') == IN_DOWN) { camMove(&camera, 2, -camSpeed); }
    camSetRendertargetEXT(&camera, displayBuffer, 1);
    camUpdate(&camera);


    // MISC
    matrix_t mvp;
    matMul(&mvp, &camera.viewProjection, &model.modelTransform);
    mdlUpdateTransform(&model);

    // SET UNIFORMS
    ubSetUniform(&shader.uniforms, 0, &mvp, sizeof(matrix_t));
    ubSetUniform(&shader.uniforms, 1, &model.modelTransform, sizeof(matrix_t));


    // RENDER
    rcDrawModel(&camera, &model, &shader);


    // PRINT PIXEL-INFO
    if(inGetKeyState('i') == IN_RELEASED) {
        int mx = inGetKeyX('i');
        int my = inGetKeyY('i');
        pixel_t *pixel = bmGetPixelAt(displayBuffer, mx, my);
        if(pixel) {
            printf("============\n");
            printf("pick at %d %d\n", mx, my);
            printf("color = %f %f %f %f\n", pixel->r, pixel->g, pixel->b, pixel->a);
            printf("depth = %f\n", pixel->z);
            printf("triangle = %d\n", pixel->triangleID);
            printf("============\n");
            g_pickedTriangle = pixel->triangleID;
        } else {
            printf("Failed: Out-of-bounds!");
        }

    }


    // SAVE TO FILE
    if(inGetKeyState('p') == IN_RELEASED) {
        char *path = "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\screenshot.png";

        bitmap_t screenshot;
        bmCreate(&screenshot, WIDTH*2, HEIGHT*2);
        bmClear(&screenshot, 0.0f, 0.0f, 0.0f);

        camCreateEXT(&screenshotCam, WIDTH*2, HEIGHT*2, 70.0, 0.1f, 100.0f, camera.pos, camera.target, camera.up);
        camSetRendertargetEXT(&screenshotCam, &screenshot, 1);
        camUpdate(&screenshotCam);

        matrix_t mvpSS;
        matMul(&mvpSS, &screenshotCam.viewProjection, &model.modelTransform);
        mdlUpdateTransform(&model);

        ubSetUniform(&shader.uniforms, 0, &mvpSS, sizeof(matrix_t));
        ubSetUniform(&shader.uniforms, 1, &model.modelTransform, sizeof(matrix_t));

        rcDrawModel(&screenshotCam, &model, &shader);

        bmSaveToFile(&screenshot, path);

        printf("screenshot saved: %s\n", path);
    }

}




void exitFunc() {
    ubFreeBuffer(&shader.uniforms);
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