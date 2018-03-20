#include "rendercontext.h"
#include "bresenham.h"
#include "model.h"
#include <math.h>
#include <stdio.h>




void rcCreateRenderData(renderdata_t *renderdata, int nObjects) {
    renderdata->shaders = calloc((size_t)nObjects, sizeof(shader_t*));
    renderdata->objects = calloc((size_t)nObjects, sizeof(model_t*));
    renderdata->cameras = calloc((size_t)nObjects, sizeof(camera_t*));
    renderdata->nObjects = nObjects;
}




// RASTERIZER

void rasterizeTriangle(camera_t *camera, model_t *model, shader_t *shader, vertex_t *v0, vertex_t *v1, vertex_t *v2) {

    bitmap_t *rendertarget = camera->rendertargets+0;

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

            // get pixel
            pixel_t *pixel = bmGetPixelAt(rendertarget, x, y);
            if(!pixel) { continue; }

            // depth test
            vec_t iplPos;
            interpolateBary(&iplPos, &v0->position, &v1->position, &v2->position, &baryCoords);
            float zPixel = pixel->z;
            float zIpl = iplPos.z;
            if(zPixel < zIpl) {
                continue;
            }
            pixel->z = zIpl;
            pixel->triangleID = v0->triangleID;

            // calc perspective correct bary coords
            float oneOverW = baryCoords.x/v0->position.w + baryCoords.y/v1->position.w + baryCoords.z/v2->position.w;
            vec_t pcBary = {0,0,0,0};
            baryCorrectPerspective(&baryCoords, v0->position.w, v1->position.w, v2->position.w, oneOverW, &pcBary);

            // interpolate vertex attributes
            vec_t iplUV, iplNrm, iplClr;
            interpolateBary(&iplUV, &v0->texCoord, &v1->texCoord, &v2->texCoord, &pcBary);
            interpolateBary(&iplNrm, &v0->normal, &v1->normal, &v2->normal, &pcBary);
            interpolateBary(&iplClr, &v0->color, &v1->color, &v2->color, &pcBary);

            vec_t *iplAttribs = NULL;
            if(v0->nAttribs > 0) {
                iplAttribs = calloc((size_t)v0->nAttribs, sizeof(vec_t));
                for(int i=0; i<v0->nAttribs; i++) {
                    interpolateBary(&iplAttribs[i], &v0->attribs[i], &v1->attribs[i], &v2->attribs[i], &baryCoords);
                }
            }

            // call fragment shader
            shader->fsh(camera, model, shader, pixel, &iplPos, &iplUV, &iplNrm, &iplClr, iplAttribs);

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
    dst->nAttribs = src->nAttribs;
    memcpy(dst->attribs, src->attribs, sizeof(vec_t)*src->nAttribs);
    dst->triangleID = src->triangleID;
}




void rcDrawModel(camera_t *camera, model_t *model, shader_t *shader) {

    // get rendertarget
    bitmap_t *rendertarget = camera->rendertargets+0;
    float rtWidth = rendertarget->width;
    float rtHeight = rendertarget->height;

    matrix_t sst = camera->screenSpaceTransform;

    // prepare output vertices
    vertex_t v0;
    vertex_t v1;
    vertex_t v2;
    v0.attribs = calloc((size_t)model->nVertexAttribs, sizeof(vec_t));
    v1.attribs = calloc((size_t)model->nVertexAttribs, sizeof(vec_t));
    v2.attribs = calloc((size_t)model->nVertexAttribs, sizeof(vec_t));


    // call pre-shader
    shader->psh(camera, model, shader);

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
        rasterizeTriangle(camera, model, shader, &v0, &v1, &v2);


    }


    free(v0.attribs);
    free(v1.attribs);
    free(v2.attribs);

}




void rcDrawRenderData(renderdata_t *renderdata) {
    for(int i=0; i<renderdata->nObjects; i++) {
        rcDrawModel(renderdata->cameras[i], renderdata->objects[i], renderdata->shaders[i]);
    }
}


