#include "display.h"
#include "input.h"
#include "rendercontext.h"
#include "textrenderer.h"
#include "objfile.h"
#include "camera.h"
#include "bitmap.h"
#include "uniforms.h"
#include "shader.h"
#include "model.h"
#include <stdio.h>
#include <windows.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 600


camera_t camera;
camera_t screenshotCam;
shader_t shaderDiablo;
shader_t shaderPlane;
model_t modelDiablo;
model_t modelPlane;
renderdata_t renderdata;




void create() {

    // MODEL DIABLO
    obj_model_t obj_diablo;
    objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose.obj", &obj_diablo);
    char *texturesDiablo[5] = {
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_diffuse.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_nm.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_nm_tangent.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_spec.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_glow.png"
    };
    mdlCreateFromObj(&obj_diablo, &modelDiablo, texturesDiablo, 5, 1);
    objFree(&obj_diablo);

    modelDiablo.translation = (vec_t){ 0,   0,  0, 0};
    modelDiablo.rotation =    (vec_t){ 0,   0,  0, 0};
    modelDiablo.scale =       (vec_t){ 10, 10, 10, 0};
    mdlUpdateTransform(&modelDiablo);


    // MODEL PLANE
    obj_model_t obj_plane;
    objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane.obj", &obj_plane);
    char *texturesPlane[5] = {
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_diffuse.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_nm.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_nm_tangent.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_spec.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_glow.png"
    };
    mdlCreateFromObj(&obj_plane, &modelPlane, texturesPlane, 5, 1);
    objFree(&obj_plane);

    modelPlane.translation = (vec_t){ 0, -10, -4, 0};
    modelPlane.rotation =    (vec_t){ 0,   0,  0, 0};
    modelPlane.scale =       (vec_t){ 13, 13, 13, 0};
    mdlUpdateTransform(&modelPlane);



    // CAMERA
    vec_t camPos = (vec_t){-22.481f, 0.0f, 3.6902f, 1.0f};
    vec_t camTgt = (vec_t){0, 0, 0, 1};
    vec_t camUp = (vec_t){0, 1, 0, 1};
    camCreateEXT(&camera, WIDTH, HEIGHT, 70.0, 0.1f, 100.0f, camPos, camTgt, camUp);


    // SHADER
    shaderDiablo.psh = pshDefault;
    shaderDiablo.vsh = vshDefault;
    shaderDiablo.fsh = fshDefault;
    ubCreateBuffer(&shaderDiablo.uniforms, 8, 2);

    shaderPlane.psh = pshDefault;
    shaderPlane.vsh = vshDefault;
    shaderPlane.fsh = fshDefault;
    ubCreateBuffer(&shaderPlane.uniforms, 8, 2);


    // RENDERDATA
    rcCreateRenderData(&renderdata, 2);
    renderdata.objects[0] = &modelDiablo;
    renderdata.objects[1] = &modelPlane;
    renderdata.shaders[0] = &shaderDiablo;
    renderdata.shaders[1] = &shaderPlane;
    renderdata.cameras[0] = &camera;
    renderdata.cameras[1] = &camera;


    // MISC
    trCreateFont();

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


    // SET UNIFORMS
    ubSetUniform(&shaderDiablo.uniforms, 1, &modelDiablo.modelTransform, sizeof(matrix_t));
    ubSetUniform(&shaderPlane.uniforms, 1, &modelPlane.modelTransform, sizeof(matrix_t));


    // RENDER
    clock_t start, end;
    start = clock();

    rcDrawRenderData(&renderdata);
//  rcDrawModel(&camera, &modelDiablo, &shaderDiablo);

    end = clock();
    int dtms = (int)(end - start);
    char *debugInfo = malloc(10 * sizeof(char));
    sprintf(debugInfo, "%dms", dtms);
    trDrawString(displayBuffer, debugInfo, 10, 20, 20, 2);


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
//    if(inGetKeyState('p') == IN_RELEASED) {
//        char *path = "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\screenshot.png";
//
//        bitmap_t screenshot;
//        bmCreate(&screenshot, WIDTH*2, HEIGHT*2);
//        bmClear(&screenshot, 0.0f, 0.0f, 0.0f);
//
//        camCreateEXT(&screenshotCam, WIDTH*2, HEIGHT*2, 70.0, 0.1f, 100.0f, camera.pos, camera.target, camera.up);
//        camSetRendertargetEXT(&screenshotCam, &screenshot, 1);
//        camUpdate(&screenshotCam);
//
//        matrix_t mvpSS;
//        matMul(&mvpSS, &screenshotCam.viewProjection, &modelDiablo.modelTransform);
//        mdlUpdateTransform(&modelDiablo);
//
//        ubSetUniform(&shader.uniforms, 0, &mvpSS, sizeof(matrix_t));
//        ubSetUniform(&shader.uniforms, 1, &modelDiablo.modelTransform, sizeof(matrix_t));
//
//        rcDrawModel(&screenshotCam, &modelDiablo, &shader);
//
//        bmSaveToFile(&screenshot, path);
//
//        printf("screenshot saved: %s\n", path);
//    }

}




void exitFunc() {
    ubFreeBuffer(&shaderDiablo.uniforms);
    ubFreeBuffer(&shaderPlane.uniforms);
    mdlFreeModel(&modelDiablo);
    mdlFreeModel(&modelPlane);
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