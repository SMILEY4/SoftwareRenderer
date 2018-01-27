#include "renderer.h"
#include "model.h"
#include "geometry.h"
#include "camera.h"
#include "bresenham.h"
#include "bitmap.h"
#include "stopwatch.h"

#include <windows.h>
#include <math.h>
#include <stdio.h>




void barycentric(vec_t *dst, vec_t *A, vec_t *B, vec_t *C, vec_t *P) {
    vec_t V1, V2, Q;
    vecSub(&V1, B, A);
    vecSub(&V2, C, A);
    vecSub(&Q , P, A);
    dst->y = vecCross2D(&Q, &V2) / vecCross2D(&V1, &V2);
    dst->z = vecCross2D(&V1, &Q) / vecCross2D(&V1, &V2);
    dst->x = 1.0f - dst->y -  dst->z;
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






void shaderVertex(renderdata_t *data, vertex_t *vertexIn, vertex_t *vertexOut, void **args) {
    void *varg0 = *(args+0);
    matrix_t *mvp = (matrix_t*)varg0;
    matTransform(&vertexOut->pos, &vertexIn->pos, mvp);
}




void shaderFragment(renderdata_t *data, triangle_t *triangle, pixel_t *pixel, vec_t *pxArgs) {

    vec_t pxCoords   = pxArgs[0];
    vec_t baryCoords = pxArgs[1];
    vec_t texCoords  = pxArgs[2];

    // get diffuse color
    bitmap_t tex_diff = data->model->texture;
    pixel_t *texDiff = bmGetPixelAt(&tex_diff, (int)(texCoords.x*tex_diff.width), (int)((1.0-texCoords.y)*tex_diff.height));

    // get normal
    vec_t mdlNormal, lightDir;
    interpolateBary(&mdlNormal, &triangle->vertices[0].normal, &triangle->vertices[1].normal, &triangle->vertices[2].normal, &baryCoords);
    lightDir = (vec_t){0.5, 1.0, 0.7};

    vecNormalize(&mdlNormal, &mdlNormal);
    vecNormalize(&lightDir, &lightDir);

    float NdotL = vecDot(&mdlNormal, &lightDir);
    NdotL = fminf(fmaxf(NdotL, 0.0f), 1.0f);

    // set final color
    if(texDiff) {
        pixel->color.r = texDiff->color.r * NdotL;
        pixel->color.g = texDiff->color.g * NdotL;
        pixel->color.b = texDiff->color.b * NdotL;
    } else {
        pixel->color.r = (texCoords.x);
        pixel->color.g = (texCoords.y);
        pixel->color.b = (texCoords.z);
    }

}




void processPixel(renderdata_t *data, bitmap_t *bitmap, int x, int y, model_t *model, triangle_t *triangle, vec_t vertices[3]) {

    pixel_t *pixel = bmGetPixelAt(bitmap, x, y);
    if(!pixel) {
        return;
    }

    // get pixel and barycentric coordinates
    vec_t pxCoords = {x+0.5f, y+0.5f, 0};
    vec_t baryCoords;
    barycentric(&baryCoords, &vertices[0], &vertices[1], &vertices[2], &pxCoords);

    vec_t texCoords;
    interpolateBary(&texCoords, &triangle->vertices[0].texcoord, &triangle->vertices[1].texcoord, &triangle->vertices[2].texcoord, &baryCoords);

    // get pixel depth
    vec_t pxDepth;
    interpolateBary(&pxDepth, &vertices[0], &vertices[1], &vertices[2], &baryCoords);
    pxCoords.z = pxDepth.z;

    // depth test
    if(pxDepth.z < pixel->depth) {
        return;
    }

    // fragment shader
    vec_t *pxArgs = calloc(3, sizeof(vec_t));
    pxArgs[0] = pxCoords;
    pxArgs[1] = baryCoords;
    pxArgs[2] = texCoords;
    shaderFragment(data, triangle, pixel, pxArgs);

    pixel->color.a = 1.0;
    pixel->depth = pxDepth.z;

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

    // setup matrices
    matrix_t screenSpaceTransform;
    matSetScreenSpaceTransform(&screenSpaceTransform, data->renderTargets[0].width/2, data->renderTargets[0].height/2);


    const unsigned int nTriangles = data->model->nTriangles;
    for(int i=nTriangles; i>0; i--) {
        triangle_t *triangle = data->model->triangles + (i-1);



        // VERTEX SPECIFICATION
        vertex_t vertexT0 = triangle->vertices[0];
        vertex_t vertexT1 = triangle->vertices[1];
        vertex_t vertexT2 = triangle->vertices[2];



        // VERTEX PROCESSING
        vertex_t vertexV0;
        vertex_t vertexV1;
        vertex_t vertexV2;
        vertex_t vertexS0;
        vertex_t vertexS1;
        vertex_t vertexS2;

        // vertex shader
        shaderVertex(data, &vertexT0, &vertexV0, data->vsArgs);
        shaderVertex(data, &vertexT1, &vertexV1, data->vsArgs);
        shaderVertex(data, &vertexT2, &vertexV2, data->vsArgs);



        // VERTEX POST PROCESSING
        matTransform(&vertexS0.pos, &vertexV0.pos, &screenSpaceTransform);
        matTransform(&vertexS1.pos, &vertexV1.pos, &screenSpaceTransform);
        matTransform(&vertexS2.pos, &vertexV2.pos, &screenSpaceTransform);
        vecPerspectiveDivide(&vertexS0.pos, &vertexS0.pos);
        vecPerspectiveDivide(&vertexS1.pos, &vertexS1.pos);
        vecPerspectiveDivide(&vertexS2.pos, &vertexS2.pos);



        // CULLING
        if(cullBackface(vertexS0.pos, vertexS1.pos, vertexS2.pos) == 1) {
            continue;
        }

        // start rasterizer
        vec_t vertices[3] = {vertexS0.pos, vertexS1.pos, vertexS2.pos};
        rasterizeTriangle(data, &data->renderTargets[0], data->model, triangle, vertices);
    }

}




void render(renderdata_t *data) {
    processVertices(data);
}








