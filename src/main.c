#include "display.h"
#include "input.h"
#include "rendercontext.h"
#include "textrenderer.h"
#include "camera.h"
#include "stopwatch.h"
#include "model.h"
#include "bitmap.h"
#include <stdio.h>
#include <time.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600


camera_t camera;
camera_t cameraShadow;
camera_t cameraHiRes;

model_t modelDiablo;
model_t modelPlane;

bitmap_t skybox;

shader_t shaderShadowPass;
shader_t shaderDefault;

renderdata_t renderdataShadowPass;
renderdata_t renderdataMainPass;

int drawShadow;



void create() {

    // MODEL DIABLO
    obj_model_t obj_diablo;
//    objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head\\african_head.obj", &obj_diablo);
//    char *texturesDiablo[5] = {
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head\\african_head_diffuse.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head\\african_head_nm.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head\\african_head_nm_tangent.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head\\african_head_spec.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\african_head\\african_head_SSS.png"
//    };

    objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose.obj", &obj_diablo);
    char *texturesDiablo[5] = {
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_diffuse.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_nm.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_nm_tangent.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_spec.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\diablo\\diablo3_pose_glow.png"
    };

//    objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\sphere.obj", &obj_diablo);
//    char *texturesDiablo[5] = {
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_diffuse.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_nm.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_nm_tangent.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_spec.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_glow.png"
//    };
    mdlCreateFromObj(&obj_diablo, &modelDiablo, texturesDiablo, 5, 2, 0);
    objFree(&obj_diablo);

    modelDiablo.translation = (vec_t){ 0, 1.75f,  0, 0};
    modelDiablo.rotation =    (vec_t){ 0,   0,  0, 0};
//    modelDiablo.scale =       (vec_t){ 0.06, 0.06, 0.06, 0};
    modelDiablo.scale =       (vec_t){ 10, 10, 10, 0};
    mdlUpdateTransform(&modelDiablo);



    // MODEL PLANE
    obj_model_t obj_plane;
    objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\groundSub.obj", &obj_plane);
    char *texturesPlane[5] = {
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_diffuse.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_nm.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_nm_tangent.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_spec.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_glow.png"
    };
    mdlCreateFromObj(&obj_plane, &modelPlane, texturesPlane, 5, 2, 1);
    objFree(&obj_plane);

    modelPlane.translation = (vec_t){ 0, -8, -4, 0};
    modelPlane.rotation =    (vec_t){ 0,  0,  0, 0};
    modelPlane.scale =       (vec_t){ 0.1f, 0.1f, 0.1f, 0};
    mdlUpdateTransform(&modelPlane);


    // ENVIRONMENT
    bmCreateFromPNG(&skybox, "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\Shiodome_Stairs_Env.png");


    // CAMERA
    vec_t camPos = (vec_t){-14.6018f, 4.0f, 17.6737f, 0.0f};
    vec_t camTgt = (vec_t){0, 0, 0, 1};
    vec_t camUp = (vec_t){0, 1, 0, 1};
    camCreateEXT(&camera, WIDTH, HEIGHT, 70.0, 0.1f, 50.0f, camPos, camTgt, camUp);

    vec_t lightPos = (vec_t){-14.6018f, 4.0f, 17.6737f, 0.0f};
//    camCreateFS(&cameraShadow, 800, 800, 1, 70.0f, 0.1f, 50.0f, lightPos, camTgt, camUp);
    camCreateFS(&cameraShadow, 400, 400, 1, 70.0f, 0.1f, 50.0f, lightPos, camTgt, camUp);
    camUpdate(&cameraShadow);

    camCreateEXT(&cameraHiRes, WIDTH*3, HEIGHT*3, 70.0, 0.1f, 50.0f, camPos, camTgt, camUp);
    camUpdate(&cameraHiRes);


    // SHADER
    shaderShadowPass.psh = pshShadowPass;
    shaderShadowPass.vsh = vshShadowPass;
    shaderShadowPass.fsh = fshShadowPass;

    shaderDefault.psh = pshDefault;
    shaderDefault.vsh = vshDefault;
    shaderDefault.fsh = fshDefault_diablo;


    // RENDERDATA
    rcCreateRenderData(&renderdataShadowPass, 2, 1, 0);
    renderdataShadowPass.objects[0] = &modelDiablo;
    renderdataShadowPass.shaders[0] = &shaderShadowPass;
    renderdataShadowPass.cameras[0] = &cameraShadow;
    renderdataShadowPass.objects[1] = &modelPlane;
    renderdataShadowPass.shaders[1] = &shaderShadowPass;
    renderdataShadowPass.cameras[1] = &cameraShadow;

    rcCreateRenderData(&renderdataMainPass, 2, 3, 2);
    renderdataMainPass.objects[0] = &modelDiablo;
    renderdataMainPass.shaders[0] = &shaderDefault;
    renderdataMainPass.cameras[0] = &camera;
    renderdataMainPass.objects[1] = &modelPlane;
    renderdataMainPass.shaders[1] = &shaderDefault;
    renderdataMainPass.cameras[1] = &camera;

    for(int i=0; i<renderdataMainPass.nObjects; i++) {
        ubSetPointer(&renderdataMainPass.buffers[i], 0, &cameraShadow);
        ubSetPointer(&renderdataMainPass.buffers[i], 1, &skybox);
    }


    // MISC
    drawShadow = 1;
    trCreateFont();
    shInit();

}



void render(bitmap_t *displayBuffer) {

    // SHADOW PASS
    if(drawShadow) {
        drawShadow = 0;
        bmClear(&cameraShadow.rendertargets[0], 1.0f, 1.0f, 1.0f, 0.0f);
        rcDrawRenderData(&renderdataShadowPass);
    }

    // MAIN PASS

//    samplesReset();
    sampleStart("mainPass");

    rcDrawRenderData(&renderdataMainPass);

    sampleEnd("mainPass");
//    sampelsPrintData();
//    samplesReset();

//    bmDrawTo(displayBuffer, cameraShadow.rendertargets+0, 1);

}





void updateFunc(bitmap_t *displayBuffer) {

    static long tick;
    tick++;

    // UPDATE CAMERA
    double camSpeed = 0.4 ;
    if(inGetKeyState('w') == IN_DOWN) { camMove(&camera, 0,  camSpeed); }
    if(inGetKeyState('s') == IN_DOWN) { camMove(&camera, 0, -camSpeed); }
    if(inGetKeyState('a') == IN_DOWN) { camMove(&camera, 1,  camSpeed); }
    if(inGetKeyState('d') == IN_DOWN) { camMove(&camera, 1, -camSpeed); }
    if(inGetKeyState('q') == IN_DOWN) { camMove(&camera, 2,  camSpeed); }
    if(inGetKeyState('e') == IN_DOWN) { camMove(&camera, 2, -camSpeed); }
    if(inGetKeyState('u') == IN_DOWN) { vecCopy(&camera.pos, &cameraShadow.pos); }
    camSetRendertargetEXT(&camera, displayBuffer, 1);
    camUpdate(&camera);

    clock_t start, end;
    start = clock();

    // DRAW
    render(displayBuffer);

    end = clock();
    int dtms = (int)(end - start);
    char *debugInfo = malloc(10 * sizeof(char));
    sprintf(debugInfo, "%dms", dtms);
    trDrawString(displayBuffer, debugInfo, 10, 300, 20, 2);


    // LOW RES RENDERING
    if(inGetKeyState(32) == IN_DOWN) {
        if(!dpIsUsingLowRes()) {
            dpUseLowRes();
        }
    } else {
        if(dpIsUsingLowRes()) {
            dpUseFullRes();
        }
    }

    // PRINT PIXEL-INFO
    if(inGetKeyState('i') == IN_RELEASED) {
        int mx = inGetKeyX('i');
        int my = inGetKeyY('i');
        pixel_t *pixel = bmGetPixelAt(displayBuffer, mx, my, 0);
        if(pixel) {
            printf("============\n");
            vecPrint(&camera.pos, "camera");
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
        bmCreate(&screenshot, WIDTH*3, HEIGHT*3);
        bmClear(&screenshot, 0.4f, 0.4f, 0.4f, 1.0);

        camSetRendertargetEXT(&cameraHiRes, &screenshot, 1);
        cameraHiRes.pos = camera.pos;
        camUpdate(&cameraHiRes);

        renderdataMainPass.cameras[0] = &cameraHiRes;
        renderdataMainPass.cameras[1] = &cameraHiRes;

        render(displayBuffer);

        renderdataMainPass.cameras[0] = &camera;
        renderdataMainPass.cameras[1] = &camera;

        bmSaveToFile(&screenshot, path);
        bmDispose(&screenshot);
        printf("screenshot saved: %s\n", path);

    }

}




void exitFunc() {
    rcFreeRenderData(&renderdataShadowPass);
    rcFreeRenderData(&renderdataMainPass);
    mdlFreeModel(&modelDiablo);
    mdlFreeModel(&modelPlane);
    camDispose(&cameraShadow);
    dpDispose();
    samplesFreeData();
}





int main(int argc, char *argv[]) {

    samplesReset();

    dpCreate(argc, argv, WIDTH, HEIGHT, 60, 2.0f);
    dpSetBackgroundColor(0.4, 0.4, 0.4, 1.0);

    dpSetUpdateFunc(&updateFunc);
    dpSetExitFunc(&exitFunc);
    create();

    dpStart();

    return 0;
}