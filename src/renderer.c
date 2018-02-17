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




void processPixel(renderdata_t *data, bitmap_t *bitmap, int x, int y, model_t *model, triangle_t *triangle, vec_t vertices[3]) {

    // get pixel
    pixel_t *pixel = bmGetPixelAt(bitmap, x, y);
    if(!pixel) {
        return;
    }

    // get pixel and barycentric coordinates
    vec_t pxCoords = {x+0.5f, y+0.5f, 0};
    vec_t baryCoords;
    barycentric(&baryCoords, &vertices[0], &vertices[1], &vertices[2], &pxCoords);

    // interpolate position
    vec_t iplPos;
    interpolateBary(&iplPos, &vertices[0], &vertices[1], &vertices[2], &baryCoords);

    // depth test
    if(iplPos.z < pixel->depth) {
        return;
    }

    // interpolate other vertex values
    vec_t *iplValues = calloc((size_t)model->nVertValuesVec3, sizeof(vec_t));
    iplValues[0] = iplPos;
    for(int i=1; i<model->nVertValuesVec3; i++) {
        vec_t in0 = triangle->vertices[0].valuesVec3[i];
        vec_t in1 = triangle->vertices[1].valuesVec3[i];
        vec_t in2 = triangle->vertices[2].valuesVec3[i];
        vec_t out;
        interpolateBary(&out, &in0, &in1, &in2, &baryCoords);
        iplValues[i] = out;
    }

    // fragment shader
    shaderFragment(iplValues, model->nVertValuesVec3, pixel, data->uniformVars);
    pixel->color.a = 1.0;
    pixel->depth = iplPos.z;

    free(iplValues);

}




void rasterizeTriangle(renderdata_t *data, bitmap_t *bitmap, model_t *model, triangle_t *triangle, vec_t *vertices) {

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

            // get pixel and barycentric coordinates
            vec_t texelCoords = {x+0.5f, y+0.5f, 0};
            vec_t baryCoords;
            barycentric(&baryCoords, &vertices[0], &vertices[1], &vertices[2], &texelCoords);

            // test if sample is part of triangle
            if(baryCoords.x < 0 || baryCoords.y < 0 || baryCoords.z < 0) {
                continue;
            }

            // draw pixel
            processPixel(data, bitmap, x, y, model, triangle, vertices);

        }
    }

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
    matSetScreenSpaceTransform(&screenSpaceTransform, data->renderTargets[0].width/2, data->renderTargets[0].height/2);

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
        shaderVertex(&vertexT0, &vertexV0, data->uniformVars);
        shaderVertex(&vertexT1, &vertexV1, data->uniformVars);
        shaderVertex(&vertexT2, &vertexV2, data->uniformVars);


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


        // CULLING
        if(cullBackface(vertexS0.valuesVec3[0], vertexS1.valuesVec3[0], vertexS2.valuesVec3[0]) == 1) {
            continue;
        }


        // start rasterizer
        vec_t vertices[3] = {vertexS0.valuesVec3[0], vertexS1.valuesVec3[0], vertexS2.valuesVec3[0]};
        rasterizeTriangle(data, &data->renderTargets[0], data->model, triangle, vertices);


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








