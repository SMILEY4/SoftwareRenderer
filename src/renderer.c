#include "renderer.h"
#include "model.h"
#include "geometry.h"
#include "camera.h"
#include "bresenham.h"
#include "bitmap.h"
#include "stopwatch.h"
#include "shader.h"

#include <windows.h>
#include <math.h>
#include <stdio.h>





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




void processPixel(renderdata_t *data, int x, int y, vertex_t vertices[3]) {

    bitmap_t *mainRenderTarget = data->camera->rendertargets;
    model_t *model = data->model;

    // get pixel
    pixel_t *pixel = bmGetPixelAt(mainRenderTarget, x, y);
    if(!pixel) {
        return;
    }

    // get pixel and barycentric coordinates
    vec_t pxCoords = {x+0.5f, y+0.5f, 0};
    vec_t baryCoords;
    barycentric(&baryCoords, &vertices[0].valuesVec3[0], &vertices[1].valuesVec3[0], &vertices[2].valuesVec3[0], &pxCoords);

    // interpolate position
    vec_t iplPos;
    interpolateBary(&iplPos, &vertices[0].valuesVec3[0], &vertices[1].valuesVec3[0], &vertices[2].valuesVec3[0], &baryCoords);

    // depth test
    if(iplPos.z > pixel->depth) {
        return;
    }

    // interpolate other vertex values
    vec_t *iplValues = calloc((size_t)model->nVertValuesVec3, sizeof(vec_t));
    iplValues[0] = iplPos;
    for(int i=1; i<model->nVertValuesVec3; i++) {
        vec_t in0 = vertices[0].valuesVec3[i];
        vec_t in1 = vertices[1].valuesVec3[i];
        vec_t in2 = vertices[2].valuesVec3[i];
        vec_t out;
        interpolateBary(&out, &in0, &in1, &in2, &baryCoords);
        iplValues[i] = out;
    }

    // fragment shader
    data->fsh(data, iplValues, model->nVertValuesVec3, pixel);
    pixel->color.a = 1.0;
    pixel->depth = iplPos.z;

    free(iplValues);

}




void rasterizeTriangle(renderdata_t *data, vertex_t vertices[3]) {

    bitmap_t *mainRenderTarget = data->camera->rendertargets;
    
    float minY = vertices[0].valuesVec3[0].y;
    minY = fminf(minY, vertices[1].valuesVec3[0].y);
    minY = fminf(minY, vertices[2].valuesVec3[0].y);
    minY = fminf(minY, mainRenderTarget->height-1);
    minY = fmaxf(minY, 0);

    float maxY = vertices[0].valuesVec3[0].y;
    maxY = fmaxf(maxY, vertices[1].valuesVec3[0].y);
    maxY = fmaxf(maxY, vertices[2].valuesVec3[0].y);
    maxY = fminf(maxY, mainRenderTarget->height-1);
    minY = fmaxf(minY, 0);

    for(int i=(int)floorf(minY); i<=(int)ceilf(maxY); i++) {
        mainRenderTarget->scanbufferMin[i] = mainRenderTarget->width+100;
        mainRenderTarget->scanbufferMax[i] = -(mainRenderTarget->width+100);
    }

    bhDrawLineToScanbuffer(mainRenderTarget->scanbufferMin, mainRenderTarget->scanbufferMax, mainRenderTarget->height, (int)vertices[0].valuesVec3[0].x, (int)vertices[0].valuesVec3[0].y, (int)vertices[1].valuesVec3[0].x, (int)vertices[1].valuesVec3[0].y);
    bhDrawLineToScanbuffer(mainRenderTarget->scanbufferMin, mainRenderTarget->scanbufferMax, mainRenderTarget->height, (int)vertices[0].valuesVec3[0].x, (int)vertices[0].valuesVec3[0].y, (int)vertices[2].valuesVec3[0].x, (int)vertices[2].valuesVec3[0].y);
    bhDrawLineToScanbuffer(mainRenderTarget->scanbufferMin, mainRenderTarget->scanbufferMax, mainRenderTarget->height, (int)vertices[1].valuesVec3[0].x, (int)vertices[1].valuesVec3[0].y, (int)vertices[2].valuesVec3[0].x, (int)vertices[2].valuesVec3[0].y);

    for(int y=(int)floorf(minY); y<=(int)ceilf(maxY); y++) {

        int startX = mainRenderTarget->scanbufferMin[y]-1;
        int endX   = mainRenderTarget->scanbufferMax[y]+1;

        for(int x=startX; x<=endX; x++) {

            // get pixel and barycentric coordinates
            vec_t texelCoords = {x+0.5f, y+0.5f, 0};
            vec_t baryCoords;
            barycentric(&baryCoords, &vertices[0].valuesVec3[0], &vertices[1].valuesVec3[0], &vertices[2].valuesVec3[0], &texelCoords);

            // test if sample is part of triangle
            if(baryCoords.x < 0 || baryCoords.y < 0 || baryCoords.z < 0) {
                continue;
            }

            // draw pixel
            processPixel(data, x, y, vertices);

        }
    }

}




float calcDepth(float n, float f, float z) {
    return (z-n)/(f-n);
}




void processVertices(renderdata_t *data) {

    // setup temp vertices
    vertex_t vertexV0;
    vertex_t vertexV1;
    vertex_t vertexV2;
    vertex_t vertexS0;
    vertex_t vertexS1;
    vertex_t vertexS2;

    unsigned int nValuesVec3 = data->model->nVertValuesVec3;
    vertexV0.valuesVec3 = calloc(nValuesVec3, sizeof(vec_t));
    vertexV1.valuesVec3 = calloc(nValuesVec3, sizeof(vec_t));
    vertexV2.valuesVec3 = calloc(nValuesVec3, sizeof(vec_t));
    vertexS0.valuesVec3 = calloc(nValuesVec3, sizeof(vec_t));
    vertexS1.valuesVec3 = calloc(nValuesVec3, sizeof(vec_t));
    vertexS2.valuesVec3 = calloc(nValuesVec3, sizeof(vec_t));

    // setup matrices
    matrix_t screenSpaceTransform;
    matSetScreenSpaceTransform(&screenSpaceTransform, data->camera->rendertargets[0].width/2, data->camera->rendertargets[0].height/2);

    // for each triangle
    const unsigned int nTriangles = data->model->nTriangles;
    for(int i=nTriangles; i>0; i--) {
        triangle_t *triangle = data->model->triangles + (i-1);


        // VERTEX SPECIFICATION
        vertex_t vertexT0 = triangle->vertices[0];
        vertex_t vertexT1 = triangle->vertices[1];
        vertex_t vertexT2 = triangle->vertices[2];

        memcpy(vertexV0.valuesVec3, vertexT0.valuesVec3, sizeof(vec_t)*nValuesVec3);
        memcpy(vertexV1.valuesVec3, vertexT1.valuesVec3, sizeof(vec_t)*nValuesVec3);
        memcpy(vertexV2.valuesVec3, vertexT2.valuesVec3, sizeof(vec_t)*nValuesVec3);


        // VERTEX PROCESSING
        data->vsh(data, &vertexT0, &vertexV0);
        data->vsh(data, &vertexT1, &vertexV1);
        data->vsh(data, &vertexT2, &vertexV2);

        // VERTEX POST PROCESSING
        memcpy(vertexS0.valuesVec3, vertexV0.valuesVec3, sizeof(vec_t)*nValuesVec3);
        memcpy(vertexS1.valuesVec3, vertexV1.valuesVec3, sizeof(vec_t)*nValuesVec3);
        memcpy(vertexS2.valuesVec3, vertexV2.valuesVec3, sizeof(vec_t)*nValuesVec3);

        matTransform(&vertexS0.valuesVec3[0], &vertexV0.valuesVec3[0], &screenSpaceTransform);
        matTransform(&vertexS1.valuesVec3[0], &vertexV1.valuesVec3[0], &screenSpaceTransform);
        matTransform(&vertexS2.valuesVec3[0], &vertexV2.valuesVec3[0], &screenSpaceTransform);

        vecPerspectiveDivide(&vertexS0.valuesVec3[0], &vertexS0.valuesVec3[0]);
        vecPerspectiveDivide(&vertexS1.valuesVec3[0], &vertexS1.valuesVec3[0]);
        vecPerspectiveDivide(&vertexS2.valuesVec3[0], &vertexS2.valuesVec3[0]);



        float zNear = data->camera->zNear;
        float zFar = data->camera->zFar;
        vertexS0.valuesVec3[0].z = calcDepth(zNear, zFar, -vertexV0.valuesVec3[0].z);
        vertexS1.valuesVec3[0].z = calcDepth(zNear, zFar, -vertexV1.valuesVec3[0].z);
        vertexS2.valuesVec3[0].z = calcDepth(zNear, zFar, -vertexV2.valuesVec3[0].z);

        // BOUNDS-CHECK + BACKFACE CULLING
        const float w = data->camera->rendertargets[0].width;
        const float h = data->camera->rendertargets[0].height;
        unsigned int nOutside = 0;
        if( vertexS0.valuesVec3[0].z < 0.0 && vertexS1.valuesVec3[0].z < 0.0 && vertexS2.valuesVec3[0].z < 0.0 ) { continue;}
        if( vertexS0.valuesVec3[0].x < 0 || vertexS0.valuesVec3[0].x >= w || vertexS0.valuesVec3[0].y < 0 || vertexS0.valuesVec3[0].y >= h ) { nOutside++; }
        if( vertexS1.valuesVec3[0].x < 0 || vertexS1.valuesVec3[0].x >= w || vertexS1.valuesVec3[0].y < 0 || vertexS1.valuesVec3[0].y >= h ) { nOutside++; }
        if( vertexS2.valuesVec3[0].x < 0 || vertexS2.valuesVec3[0].x >= w || vertexS2.valuesVec3[0].y < 0 || vertexS2.valuesVec3[0].y >= h ) { nOutside++; }

        int cullBF = 0;
        if(data->cullingMode == 1) { // backface
            cullBF = cullBackface(vertexS0.valuesVec3[0], vertexS1.valuesVec3[0], vertexS2.valuesVec3[0]);
        } else {
            cullBF = cullBackface(vertexS0.valuesVec3[0], vertexS1.valuesVec3[0], vertexS2.valuesVec3[0]) == 0 ? 1 : 0;
        }

        if(nOutside == 3 || cullBF) {
           continue;
        }


        // START RASTERIZER
        vertex_t vertices[3] = {vertexS0, vertexS1, vertexS2};
        rasterizeTriangle(data, vertices);

    }


    // free temp vertices
    free(vertexV0.valuesVec3);
    free(vertexV1.valuesVec3);
    free(vertexV2.valuesVec3);
    free(vertexS0.valuesVec3);
    free(vertexS1.valuesVec3);
    free(vertexS2.valuesVec3);

}




void render(renderdata_t *data) {
    processVertices(data);
}








