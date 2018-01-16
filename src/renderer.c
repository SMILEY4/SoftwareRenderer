#include "renderer.h"
#include "model.h"
#include "bitmap.h"
#include "bresenham.h"
#include "camera.h"
#include "geometry.h"
#include <math.h>
#include <stdio.h>

#define toRadians(angleDegrees) ((angleDegrees) * M_PI / 180.0)
#define toDegrees(angleRadians) ((angleRadians) * 180.0 / M_PI




void srInit(float width, float height) {
    srCamera.pos = (vec_t){0, 0, -20, 1};
    srCamera.target = (vec_t){0, 0, 0, 1};
    srCamera.up = (vec_t){0, 1, 0, 1};
    matSetPerspective(&srCamera.projection, toRadians(70.0), width/height, 0.1, 1000.0);
    camUpdate(&srCamera);
}




void srRenderWireframe(bitmap_t *bitmap, model_t *model) {

    float width = bitmap->width;
    float height = bitmap->height;

    // create view projection
    matrix_t viewProjection = srCamera.viewProjection;

    // create model transform
    static float rotCounter; rotCounter = 1.0;
    matrix_t translation, rotation, scale, modelTransform;
    matSetTranslation(&translation, 0, 0, 0);
    matSetRotation(&rotation, 0, rotCounter, 0);
    matSetScale(&scale, 10, -10, 10);
    matMul3(&modelTransform, &translation, &rotation, &scale);

    // create model view projection
    matrix_t mvp;
    matMul(&mvp, &viewProjection, &modelTransform);

    // create sceenSpaceTransform
    matrix_t screenSpaceTransform;
    matSetScreenSpaceTransform(&screenSpaceTransform, bitmap->width/2, bitmap->height/2);

    // render model
    for(int i=0; i<model->nTriangles; i++) {
        triangle_t *triangle = model->triangles+i;

        // transform vertices
        vec_t p[3];
        for(int j=0; j<3; j++) {
            matTransform(&p[j], &triangle->vertices[j].pos, &mvp);
            matTransform(&p[j], &p[j], &screenSpaceTransform);
            vecPerspectiveDivide(&p[j], &p[j]);
        }

        // draw triangle
        bhDrawTriangle(bitmap,
                       (int)p[0].x, (int)p[0].y,
                       (int)p[1].x, (int)p[1].y,
                       (int)p[2].x, (int)p[2].y,
                       (color_t){1, 0, 0, 1},
                       (color_t){0, 1, 0, 1},
                       (color_t){0, 0, 1, 1}
        );


    }

}
















