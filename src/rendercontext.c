#include "rendercontext.h"
#include "bresenham.h"

#include <math.h>




// FRAGMENT PROCESSING






// RASTERIZER

float calcDepth(float z, float zNear, float zFar) {
    return (2.0f*zFar*zNear) / (zFar*(-z) + zFar + zNear*z + zNear);
}


void rasterizeTriangle(bitmap_t *rendertarget, bitmap_t *texture, vertex_t *v0, vertex_t *v1, vertex_t *v2) {

    // calculate bounding
    float minY = v0->position.y;
    minY = fminf(minY, v1->position.y);
    minY = fminf(minY, v2->position.y);
    minY = fminf(minY, rendertarget->height-1);
    minY = fmaxf(minY, 0);

    float maxY = v0->position.y;
    maxY = fmaxf(maxY, v1->position.y);
    maxY = fmaxf(maxY, v2->position.y);
    maxY = fminf(maxY, rendertarget->height-1);
    minY = fmaxf(minY, 0);

    // clear scanbuffer
    for(int i=(int)floorf(minY); i<=(int)ceilf(maxY); i++) {
        rendertarget->scanbufferMin[i] = rendertarget->width+100;
        rendertarget->scanbufferMax[i] = -(rendertarget->width+100);
    }

    // draw scanbuffer
    bhDrawLineToScanbuffer(rendertarget->scanbufferMin, rendertarget->scanbufferMax, rendertarget->height, (int)v0->position.x, (int)v0->position.y, (int)v1->position.x, (int)v1->position.y);
    bhDrawLineToScanbuffer(rendertarget->scanbufferMin, rendertarget->scanbufferMax, rendertarget->height, (int)v0->position.x, (int)v0->position.y, (int)v2->position.x, (int)v2->position.y);
    bhDrawLineToScanbuffer(rendertarget->scanbufferMin, rendertarget->scanbufferMax, rendertarget->height, (int)v1->position.x, (int)v1->position.y, (int)v2->position.x, (int)v2->position.y);

    // rasterize triangle
    vec_t texelCoords, baryCoords;
    for(int y=(int)floorf(minY); y<=(int)ceilf(maxY); y++) {

        int startX = (int)fmaxf(rendertarget->scanbufferMin[y]-1, 0.0f);
        int endX   = (int)fminf(rendertarget->scanbufferMax[y]+1, rendertarget->width);

        for(int x=startX; x<=endX; x++) {

            texelCoords = (vec_t){x+0.5f, y+0.5f, 0, 0};
            barycentric(&baryCoords, &v0->position, &v1->position, &v2->position, &texelCoords);

            // test if sample is part of triangle
            if(baryCoords.x < 0 || baryCoords.y < 0 || baryCoords.z < 0) {
                continue;
            }

            // draw pixel
            pixel_t *pixel = bmGetPixelAt(rendertarget, x, y);
            if(!pixel) { continue; }

            // depth test
            vec_t posIpl; interpolateBary(&posIpl, &v0->position, &v1->position, &v2->position, &baryCoords);
            float zPixel = pixel->z;
            float zIpl = posIpl.z;
            if(zPixel < zIpl) {
                continue;
            }
            pixel->z = zIpl;


            // calc perspective correct bary coords
            float dPx = calcDepth(zIpl, 0.1f, 100.0f);
            texelCoords = (vec_t){texelCoords.x*dPx, texelCoords.y*dPx, 0.0, 0.0};
            vec_t a0 = {v0->position.x*v0->position.w, v0->position.y*v0->position.w, 0.0, 0.0};
            vec_t a1 = {v1->position.x*v1->position.w, v1->position.y*v1->position.w, 0.0, 0.0};
            vec_t a2 = {v2->position.x*v2->position.w, v2->position.y*v2->position.w, 0.0, 0.0};
            barycentric(&baryCoords, &a0, &a1, &a2, &texelCoords);

            // get texture
            vec_t texCoords;  interpolateBary(&texCoords, &v0->texCoord, &v1->texCoord, &v2->texCoord, &baryCoords);
            pixel_t *sample = bmGetPixelUV(texture, texCoords.x, texCoords.y);
            if(sample) {
                pixel->r = sample->r;
                pixel->g = sample->g;
                pixel->b = sample->b;
                pixel->a = 1.0f;
            }

        }
    }


}






// VERTEX PROCESSING

int cullBackface(vec_t *A, vec_t *B, vec_t *C) {
    vec_t triNormal;
    vec_t triAB, triAC;
    vecSub(&triAB, B, A);
    vecSub(&triAC, C, A);
    vecCross(&triNormal, &triAB, &triAC);
    triNormal.w = 0.0f;
    vecNormalize(&triNormal, &triNormal);
    float d = vecDot(&triNormal, &(vec_t){0.0f, 0.0f, 1.0f, 0.0f});
    if(d < 0) {
        return 1;
    } else {
        return 0;
    }
}




void projectVertex(vertex_t *vertexOut, vertex_t *vertexIn, matrix_t *sst) {
    matTransform(&vertexOut->position, &vertexIn->position, sst);
    vecPerspectiveDivide(&vertexOut->position, &vertexOut->position);
}



void copyVertex(vertex_t *dst, vertex_t *src) {
    dst->position = src->position;
    dst->normal = src->normal;
    dst->texCoord = src->texCoord;
    dst->color = src->color;
}


void rcDrawModel(camera_t *camera, model_t *model, shader_t *shader) {

    // get rendertarget
    bitmap_t *rendertarget = camera->rendertargets;
    float rtWidth = rendertarget->width;
    float rtHeight = rendertarget->height;

    matrix_t sst = camera->screenSpaceTransform;

    vertex_t v0;
    vertex_t v1;
    vertex_t v2;

    // for each triangle
    for(int i=0; i<model->nTriangles; i++) {
        triangle_t *triangle = model->triangles+i;

        // get vertices
        vertex_t *vo0 = triangle->vertices+0;
        vertex_t *vo1 = triangle->vertices+1;
        vertex_t *vo2 = triangle->vertices+2;

        // copy values
        copyVertex(&v0, vo0);
        copyVertex(&v1, vo1);
        copyVertex(&v2, vo2);

        // transform / project vertices
        shader->vsh(vo0, &v0, shader);
        shader->vsh(vo1, &v1, shader);
        shader->vsh(vo2, &v2, shader);

        projectVertex(&v0, &v0, &sst);
        projectVertex(&v1, &v1, &sst);
        projectVertex(&v2, &v2, &sst);

        // cull depth
        if( (v0.position.z < 0 || v0.position.z > 1) || (v1.position.z < 0 || v1.position.z > 1) || (v2.position.z < 0 || v2.position.z > 1) ) {
            continue;
        }

        // cull viewspace
        int nOutside = 0;
        if( (v0.position.x < 0 || v0.position.x > rtWidth) || (v0.position.y < 0 || v0.position.y > rtHeight) ) { nOutside++; }
        if( (v1.position.x < 0 || v1.position.x > rtWidth) || (v1.position.y < 0 || v1.position.y > rtHeight) ) { nOutside++; }
        if( (v2.position.x < 0 || v2.position.x > rtWidth) || (v2.position.y < 0 || v2.position.y > rtHeight) ) { nOutside++; }
        if(nOutside == 3) {
            continue;
        }

        // cull backspace
        if(cullBackface(&v0.position, &v1.position, &v2.position)) {
            continue;
        }

        // rasterize
        rasterizeTriangle(rendertarget, &model->textures[0], &v0, &v1, &v2);

        // draw wireframe
//        bhDrawLine(rendertarget, (int)v0.position.x, (int)v0.position.y, (int)v1.position.x, (int)v1.position.y, 1.0f, 1.0f, 1.0f);
//        bhDrawLine(rendertarget, (int)v0.position.x, (int)v0.position.y, (int)v2.position.x, (int)v2.position.y, 1.0f, 1.0f, 1.0f);
//        bhDrawLine(rendertarget, (int)v1.position.x, (int)v1.position.y, (int)v2.position.x, (int)v2.position.y, 1.0f, 1.0f, 1.0f);

    }


}