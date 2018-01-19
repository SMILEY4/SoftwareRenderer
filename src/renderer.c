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
    dst->x = 1.0f - dst->y -  dst->z;
}




int isInVisibleArea(float width, float height, vec_t *A, vec_t *B, vec_t *C) {
    int visible = 0;
    if( (0 <= A->x && A->x < width) && (0 <= A->y && A->y < height) ) { visible = 1; }
    if( (0 <= B->x && B->x < width) && (0 <= B->y && B->y < height) ) { visible = 1; }
    if( (0 <= C->x && C->x < width) && (0 <= C->y && C->y < height) ) { visible = 1; }
    return visible;
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




void processPixel(bitmap_t *bitmap, int x, int y, model_t *model, triangle_t *triangle, vec_t vertices[3]) {

    pixel_t *pixel = bmGetPixelAt(bitmap, x, y);
    if(!pixel) {
        return;
    }

    // get texel and barycentric coordinates
    vec_t texelCoords = {x+0.5, y+0.5, 0};
    vec_t baryCoords;
    barycentric(&baryCoords, &vertices[0], &vertices[1], &vertices[2], &texelCoords);

    // get pixel depth
    vec_t pxDepth;
    interpolateBary(&pxDepth, &vertices[0], &vertices[1], &vertices[2], &baryCoords);

    // depth test
    if(pxDepth.z < pixel->depth) {
        return;
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




void drawTriangle(bitmap_t *bitmap, model_t *model, triangle_t *triangle, vec_t vertices[3]) {

    if(isInVisibleArea(bitmap->width, bitmap->height, &vertices[0], &vertices[1], &vertices[2]) == 0) {
        return;
    }
    if(cullBackface(vertices[0], vertices[1], vertices[2]) == 1) {
        return;
    }

    float minY = vertices[0].y;
    minY = fminf(minY, vertices[1].y);
    minY = fminf(minY, vertices[2].y);
    minY = fminf(minY, bitmap->height-1);
    minY = fmaxf(minY, 0);


    float maxY = vertices[0].y;
    maxY = fmaxf(maxY, vertices[1].y);
    maxY = fmaxf(maxY, vertices[2].y);
    maxY = fminf(maxY, bitmap->height-1);
    minY = fmaxf(minY, 0);

    for(int i=(int)floorf(minY); i<=(int)ceilf(maxY); i++) {
        bitmap->scanbufferMin[i] = bitmap->width+100;
        bitmap->scanbufferMax[i] = -(bitmap->width+100);

    }

    bhDrawLineToScanbuffer(bitmap->scanbufferMin, bitmap->scanbufferMax, bitmap->height, (int)vertices[0].x, (int)vertices[0].y, (int)vertices[1].x, (int)vertices[1].y);
    bhDrawLineToScanbuffer(bitmap->scanbufferMin, bitmap->scanbufferMax, bitmap->height, (int)vertices[0].x, (int)vertices[0].y, (int)vertices[2].x, (int)vertices[2].y);
    bhDrawLineToScanbuffer(bitmap->scanbufferMin, bitmap->scanbufferMax, bitmap->height, (int)vertices[1].x, (int)vertices[1].y, (int)vertices[2].x, (int)vertices[2].y);


    for(int y=(int)floorf(minY); y<=(int)ceilf(maxY); y++) {

        int startX = bitmap->scanbufferMin[y]-1;
        int endX   = bitmap->scanbufferMax[y]+1;

        for(int x=startX; x<=endX; x++) {

            // get texel and barycentric coordinates
            vec_t texelCoords = {x+0.5f, y+0.5f, 0};
            vec_t baryCoords;
            barycentric(&baryCoords, &vertices[0], &vertices[1], &vertices[2], &texelCoords);

            // test if sample is part of triangle
            if(baryCoords.x < 0 || baryCoords.y < 0 || baryCoords.z < 0) {
                continue;
            }

            // draw pixel
            processPixel(bitmap, x, y, model, triangle, vertices);

        }
    }

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











