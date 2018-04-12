#include "display.h"
#include "input.h"
#include "rendercontext.h"
#include "textrenderer.h"
#include "camera.h"
#include "stopwatch.h"
#include "model.h"
#include "bitmap.h"
#include "shaderutils.h"
#include <stdio.h>
#include <time.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600

#define SCREENSHOT_SCALE 3

camera_t camera;
camera_t cameraShadow;
camera_t cameraHiRes;

model_t modelMain;
model_t modelPlane;

bitmap_t heightmap;

bitmap_t skybox;

shader_t shaderShadowPass;
shader_t shaderDefault;

renderdata_t renderdataShadowPass;
renderdata_t renderdataMainPass;

int drawShadow;



void create() {

    unsigned int nTextures = 5;

    // MODEL MAIN
    obj_model_t obj_model;
    objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\sphere\\sphere6b.obj", &obj_model);

    mdlCreateFromObj(&obj_model, &modelMain, NULL, nTextures, 2, 0);
    objFree(&obj_model);

    modelMain.translation = (vec_t){ 0, 0, 0, 0};
    modelMain.rotation =    (vec_t){ 0, 0, 0, 0};
    modelMain.scale =       (vec_t){ 0.035, 0.035, 0.035, 0};
    mdlUpdateTransform(&modelMain);


    // MODEL GROUND
    obj_model_t obj_plane;
    objParse("D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\groundSub3.obj", &obj_plane);

    mdlCreateFromObj(&obj_plane, &modelPlane, NULL, nTextures, 2, 1);
    objFree(&obj_plane);

    modelPlane.translation = (vec_t){ 0, -8, -4, 0};
    modelPlane.rotation =    (vec_t){ 0,  0,  0, 0};
    modelPlane.scale =       (vec_t){ 0.13f, 0.1f, 0.13f, 0};
    mdlUpdateTransform(&modelPlane);

    bmCreateFromPNG(&heightmap,  "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_glow.png");


    // LOAD TEXTURES
//    char *texturePaths[5] = {
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\agedplanks1-ue\\agedplanks1-albedo.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\agedplanks1-ue\\agedplanks1-normal4-ue.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\agedplanks1-ue\\agedplanks1-roughness.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\agedplanks1-ue\\agedplanks1-ao.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_glow.png",
//    };
//    char *texturePaths[5] = {
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\greasy-pan-2-Unreal-Engine\\greasy-pan-2-albedo.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\greasy-pan-2-Unreal-Engine\\greasy-pan-2-normal.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\greasy-pan-2-Unreal-Engine\\greasy-pan-2-roughness.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_diffuse.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\greasy-pan-2-Unreal-Engine\\greasy-pan-2-metal.png",
//    };
//    char *texturePaths[5] = {
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\cratered-rock-Unreal-Engine\\cratered-rock-albedo.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\cratered-rock-Unreal-Engine\\cratered-rock-normal.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\cratered-rock-Unreal-Engine\\cratered-rock-roughness.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\cratered-rock-Unreal-Engine\\cratered-rock-ao.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\cratered-rock-Unreal-Engine\\cratered-rock-metalness.png",
//    };
//    bmCreateFromPNG(&heightmap,  "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\cratered-rock-Unreal-Engine\\cratered-rock-height.png");

//    char *texturePaths[5] = {
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\harshbricks-Unreal-Engine\\harshbricks-albedo.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\harshbricks-Unreal-Engine\\harshbricks-normal.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\harshbricks-Unreal-Engine\\harshbricks-roughness.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\harshbricks-Unreal-Engine\\harshbricks-ao2.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\harshbricks-Unreal-Engine\\harshbricks-metalness.png",
//    };
//    bmCreateFromPNG(&heightmap,  "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\harshbricks-Unreal-Engine\\harshbricks-height5-16.png");
//    char *texturePaths[5] = {
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\mixedmoss-ue4\\mixedmoss-albedo2.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\mixedmoss-ue4\\mixedmoss-normal2.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\mixedmoss-ue4\\mixedmoss-roughness.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\mixedmoss-ue4\\mixedmoss-ao2.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\mixedmoss-ue4\\mixedmoss-metalness.png",
//    };
//    char *texturePaths[5] = {
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\rustediron1-alt2-Unreal-Engine\\rustediron2_basecolor.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\rustediron1-alt2-Unreal-Engine\\rustediron2_normal.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\rustediron1-alt2-Unreal-Engine\\rustediron2_roughness.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_diffuse.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\rustediron1-alt2-Unreal-Engine\\rustediron2_metallic.png",
//    };
//        char *texturePaths[5] = {
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\bathroomtile1-dx\\bathroomtile1_basecolor.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\bathroomtile1-dx\\bathroomtile1_normal-dx.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\bathroomtile1-dx\\bathroomtile1_roughness.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\bathroomtile1-dx\\bathroomtile1_ao.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\bathroomtile1-dx\\bathroomtile1_metalness.png",
//    };
//    char *texturePaths[5] = {
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\dry-dirt2-Unreal-Engine-2\\dry-dirt2-albedo.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\dry-dirt2-Unreal-Engine-2\\dry-dirt2-normal2.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\dry-dirt2-Unreal-Engine-2\\dry-dirt2-roughness.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\dry-dirt2-Unreal-Engine-2\\dry-dirt2-ao.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\dry-dirt2-Unreal-Engine-2\\dry-dirt2-metalness.png",
//    };
//    char *texturePaths[5] = {
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\leaf-fall1-ue4\\leaf-fall1-albedo.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\leaf-fall1-ue4\\leaf-fall3-normal-ue.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\leaf-fall1-ue4\\leaf-fall1-roughness.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\leaf-fall1-ue4\\leaf-fall1-ao.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\leaf-fall1-ue4\\leaf-fall1-metalness.png",
//    };
//    char *texturePaths[5] = {
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\wornpaintedcement-Unreal_Engine\\wornpaintedcement-albedo.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\wornpaintedcement-Unreal_Engine\\wornpaintedcement-norrmal.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\wornpaintedcement-Unreal_Engine\\wornpaintedcement-roughness.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\wornpaintedcement-Unreal_Engine\\wornpaintedcement-ao.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\wornpaintedcement-Unreal_Engine\\wornpaintedcement-metalness.png",
//    };
//    char *texturePaths[5] = {
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\bamboo-wood-semigloss-Unreal-Engine\\bamboo-wood-semigloss-albedo.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\bamboo-wood-semigloss-Unreal-Engine\\bamboo-wood-semigloss-normal.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\bamboo-wood-semigloss-Unreal-Engine\\bamboo-wood-semigloss-roughness.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\bamboo-wood-semigloss-Unreal-Engine\\bamboo-wood-semigloss-ao.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\bamboo-wood-semigloss-Unreal-Engine\\bamboo-wood-semigloss-metal.png",
//    };
//    char *texturePaths[5] = {
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\darktiles1-ue4\\darktiles1_basecolor.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\darktiles1-ue4\\darktiles1_normal-DX.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\darktiles1-ue4\\darktiles1_roughness.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\darktiles1-ue4\\darktiles1_AO.png",
//            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\darktiles1-ue4\\darktiles1_metallic.png",
//    };
    char *texturePaths[5] = {
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\gold-scuffed-Unreal-Engine\\gold-scuffed_basecolor.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\gold-scuffed-Unreal-Engine\\gold-scuffed_normal.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\gold-scuffed-Unreal-Engine\\gold-scuffed_roughness.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\plane\\plane_diffuse.png",
            "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\materials\\gold-scuffed-Unreal-Engine\\gold-scuffed_metallic.png",
    };

    for(int i=0; i<nTextures; i++) {
        char *filename = texturePaths[i];
        bitmap_t texture;
        bmCreateFromPNG(&texture, filename);
        memcpy(modelMain.textures+i, &texture, sizeof(texture));
        memcpy(modelPlane.textures+i, &texture, sizeof(texture));
    }


    bmCreateFromPNG(&skybox, "D:\\LukasRuegner\\Programmieren\\C\\SoftwareRenderer\\res\\skybox\\Shiodome_Stairs.png");


    // CAMERA
    vec_t camPos = (vec_t){4.4036f, 0.0539f, 12.7757f, 0.0f};
    vec_t camTgt = (vec_t){0, 0, 0, 1};
    vec_t camUp = (vec_t){0, 1, 0, 1};
    camCreateEXT(&camera, WIDTH, HEIGHT, 70.0, 0.1f, 50.0f, camPos, camTgt, camUp);

    vec_t lightPos = (vec_t){-4.1879f, 7.6163, 9.3500, 0.0f};
    camCreateFS(&cameraShadow, 800, 800, 1, 70.0f, 0.1f, 50.0f, lightPos, camTgt, camUp);
    camUpdate(&cameraShadow);

    camCreateEXT(&cameraHiRes, WIDTH*SCREENSHOT_SCALE, HEIGHT*SCREENSHOT_SCALE, 70.0, 0.1f, 50.0f, camPos, camTgt, camUp);
    camUpdate(&cameraHiRes);


    // SHADER
    shaderShadowPass.psh = pshShadowPass;
    shaderShadowPass.vsh = vshShadowPass;
    shaderShadowPass.fsh = fshShadowPass;

    shaderDefault.psh = pshDefault;
    shaderDefault.vsh = vshDefault;
    shaderDefault.fsh = fshDefault_diablo;


    // RENDERDATA
    rcCreateRenderData(&renderdataShadowPass, 2, 2, 1);
    renderdataShadowPass.objects[0] = &modelMain;
    renderdataShadowPass.shaders[0] = &shaderShadowPass;
    renderdataShadowPass.cameras[0] = &cameraShadow;
    renderdataShadowPass.objects[1] = &modelPlane;
    renderdataShadowPass.shaders[1] = &shaderShadowPass;
    renderdataShadowPass.cameras[1] = &cameraShadow;

    for(int i=0; i<renderdataShadowPass.nObjects; i++) {
        ubSetPointer(&renderdataShadowPass.buffers[i], 0, &heightmap);
    }

    rcCreateRenderData(&renderdataMainPass, 2, 3, 3);
    renderdataMainPass.objects[0] = &modelMain;
    renderdataMainPass.shaders[0] = &shaderDefault;
    renderdataMainPass.cameras[0] = &camera;
    renderdataMainPass.objects[1] = &modelPlane;
    renderdataMainPass.shaders[1] = &shaderDefault;
    renderdataMainPass.cameras[1] = &camera;

    for(int i=0; i<renderdataMainPass.nObjects; i++) {
        ubSetPointer(&renderdataMainPass.buffers[i], 0, &cameraShadow);
        ubSetPointer(&renderdataMainPass.buffers[i], 1, &heightmap);
        ubSetPointer(&renderdataMainPass.buffers[i], 2, &skybox);
    }



    // MISC
    drawShadow = 1;
    trCreateFont();
    initShaderUtils();

}



void render(bitmap_t *displayBuffer) {

    // SHADOW PASS
    if(drawShadow) {
        drawShadow = 0;
        bmClear(&cameraShadow.rendertargets[0], 1.0f, 1.0f, 1.0f, 0.0f);
        rcDrawRenderData(&renderdataShadowPass, 0);
    }
    // MAIN PASS
    rcDrawRenderData(&renderdataMainPass, 0);
//    rcDrawRenderData(&renderdataMainPass, 1);

    rough = 0.0;

}





void updateFunc(bitmap_t *displayBuffer) {

    if(inGetKeyState(27) == IN_DOWN) { exit(0); } // 27 = escape

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
    mdlFreeModel(&modelMain);
//    mdlFreeModel(&modelPlane);
//    bmDispose(&envmap);
    camDispose(&cameraShadow);
    dpDispose();
}





int main(int argc, char *argv[]) {

    dpCreate(argc, argv, WIDTH, HEIGHT, 60, 2.0f);
    dpSetBackgroundColor(0.4, 0.4, 0.4, 1.0);

    dpSetUpdateFunc(&updateFunc);
    dpSetExitFunc(&exitFunc);
    create();

    dpStart();

    return 0;
}