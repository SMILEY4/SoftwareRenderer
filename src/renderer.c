#include "renderer.h"
#include "model.h"
#include "bitmap.h"
#include "bresenham.h"
#include "camera.h"
#include "geometry.h"
#include "stopwatch.h"
#include <math.h>
#include <stdio.h>
#include <windows.h>

#define toRadians(angleDegrees) ((angleDegrees) * M_PI / 180.0)
#define toDegrees(angleRadians) ((angleRadians) * 180.0 / M_PI




void barycentric(vec_t *dst, vec_t *A, vec_t *B, vec_t *C, vec_t *P) {
    vec_t V1, V2, Q;
    vecSub(&V1, B, A);
    vecSub(&V2, C, A);
    vecSub(&Q , P, A);
    dst->y = vecCross2D(&Q, &V2) / vecCross2D(&V1, &V2);
    dst->z = vecCross2D(&V1, &Q) / vecCross2D(&V1, &V2);
    dst->x = 1.0 - dst->y -  dst->z;
}




int cullBackface(vec_t A, vec_t B, vec_t C) {
    vec_t triNormal;
    vec_t triAB, triAC;
    vecSub(&triAB, &B, &A);
    vecSub(&triAC, &C, &A);
    vecCross(&triNormal, &triAB, &triAC);
    vecNormalize(&triNormal, &triNormal);
    float d = vecDot(&triNormal, &(vec_t){0, 0, 1});
    if(d < 0) {
        return 1;
    } else {
        return 0;
    }
}




void interpolateBary(vec_t *dst, vec_t *A, vec_t *B, vec_t *C, vec_t *baryCoords) {
    dst->x = A->x * baryCoords->x  +  B->x * baryCoords->y  +  C->x * baryCoords->z;
    dst->y = A->y * baryCoords->x  +  B->y * baryCoords->y  +  C->y * baryCoords->z;
    dst->z = A->z * baryCoords->x  +  B->z * baryCoords->y  +  C->z * baryCoords->z;
    dst->w = A->w * baryCoords->x  +  B->w * baryCoords->y  +  C->w * baryCoords->z;
}




void drawTriangle(bitmap_t *bitmap, model_t *model, triangle_t *triangle, vec_t vertices[3]) {

    if(cullBackface(vertices[0], vertices[1], vertices[2]) == 1) {
        return;
    }

    float minX = bitmap->width-1;
    float minY = bitmap->height-1;
    float maxX = 0;
    float maxY = 0;

    for(int i=0; i<3; i++) {
        vec_t v = vertices[i];
        v.z = 0.0;
        v.w = 0.0;
        if(v.x < minX) { minX = v.x; }
        if(v.y < minY) { minY = v.y; }
        if(v.x > maxX) { maxX = v.x; }
        if(v.y > maxY) { maxY = v.y; }
    }

    unsigned int pixelsHit = 0;
    unsigned int pixelsMiss;

    for (int x = (int) floor(minX); x <= (int) ceil(maxX); x++) {
        for (int y = (int) floor(minY); y <= (int) ceil(maxY); y++) {

            // get texel and barycentric coordinates
            vec_t texelCoords = {x+0.5, y+0.5, 0};
            vec_t baryCoords;
            barycentric(&baryCoords, &vertices[0], &vertices[1], &vertices[2], &texelCoords);

            // test if sample is part of triangle
            if(baryCoords.x < 0 || baryCoords.y < 0 || baryCoords.z < 0) {
                continue;
                pixelsMiss += 1;
            }
            pixelsHit += 1;


            pixel_t *pixel = bmGetPixelAt(bitmap, x, y);

            // get pixel depth
            vec_t pxDepth;
            interpolateBary(&pxDepth, &vertices[0], &vertices[1], &vertices[2], &baryCoords);

            // second depth test
            if(pxDepth.z < pixel->depth) {
                continue;
            }

            // texCoord of pixel
            vec_t pxUV;
            interpolateBary(&pxUV, &triangle->vertices[0].texcoord, &triangle->vertices[1].texcoord, &triangle->vertices[2].texcoord, &baryCoords);

            // get diffuse color
            pixel_t *texDiff = bmGetPixelAt(&model->texture, (int)(pxUV.x*model->texture.width), (int)((1.0-pxUV.y)*model->texture.height));

            // draw pixel
            if(texDiff) {
                pixel->color.r = texDiff->color.r;
                pixel->color.g = texDiff->color.g;
                pixel->color.b = texDiff->color.b;
            } else {
                pixel->color.r = (float)(pxUV.x);
                pixel->color.g = (float)(pxUV.y);
                pixel->color.b = (float)(pxUV.z);
            }
            pixel->color.a = 1.0;
            pixel->depth = vertices[0].z;

        }
    }

    printf("pixel stats: total: %d,  hit: %d,  miss: %d,   percHit: %f \n", pixelsHit+pixelsMiss, pixelsHit, pixelsMiss, (float)pixelsHit / (float)(pixelsHit+pixelsMiss));

}



void srRender(bitmap_t *bitmap, model_t *model) {

    // create view projection
    matrix_t viewProjection = srCamera.viewProjection;

    // create model transform
    matrix_t modelTransform = model->modelTransform;

    // create model view projection
    matrix_t mvp;
    matMul(&mvp, &viewProjection, &modelTransform);

    // create sceenSpaceTransform
    matrix_t screenSpaceTransform;
    matSetScreenSpaceTransform(&screenSpaceTransform, bitmap->width/2, bitmap->height/2);


    // render model
    //for(int i=0; i<model->nTriangles; i++) {
    for(int i=model->nTriangles; i>0; i--) {

        watchStart("triangle");

        triangle_t *triangle = model->triangles + (i-1);

        // transform vertices
        watchStart("tran_triangle");
        vec_t p[3];
        for(int j=0; j<3; j++) {
            matTransform(&p[j], &triangle->vertices[j].pos, &mvp);
            matTransform(&p[j], &p[j], &screenSpaceTransform);
            vecPerspectiveDivide(&p[j], &p[j]);
        }
        watchEnd("tran_triangle");


        // draw triangle
        watchStart("draw_triangle");
        drawTriangle(bitmap, model, triangle, p);
        watchEnd("draw_triangle");


        watchEnd("triangle");
    }
}




void srRenderWireframe(bitmap_t *bitmap, model_t *model) {

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




void srInit(float width, float height) {
    srCamera.pos = (vec_t){0, 0, -20, 1};
    srCamera.target = (vec_t){0, 0, 0, 1};
    srCamera.up = (vec_t){0, 1, 0, 1};
    matSetPerspective(&srCamera.projection, toRadians(70.0), width/height, 0.1, 1000.0);
    camUpdate(&srCamera);
}











