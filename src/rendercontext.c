#include "rendercontext.h"
#include "bresenham.h"
#include "model.h"
#include "stopwatch.h"
#include "bitmap.h"
#include "geometry.h"
#include "input.h"
#include <math.h>
#include <stdio.h>
#include <pthread.h>

#define USE_THREADS
#define N_THREADS 8



void rcCreateRenderData(renderdata_t *renderdata, int nObjects, unsigned int ubMaxElements, unsigned int ubMaxPointers) {
    renderdata->shaders = calloc((size_t)nObjects, sizeof(shader_t*));
    renderdata->objects = calloc((size_t)nObjects, sizeof(model_t*));
    renderdata->cameras = calloc((size_t)nObjects, sizeof(camera_t*));
    renderdata->buffers = calloc((size_t)nObjects, sizeof(uniformbuffer_t));
    for(int i=0; i<nObjects; i++) {
        ubCreateBuffer(renderdata->buffers+i, ubMaxElements, ubMaxPointers);
    }
    renderdata->nObjects = nObjects;
}




void rcFreeRenderData(renderdata_t *renderdata) {
    for(int i=0; i<renderdata->nObjects; i++) {
        ubFreeBuffer(renderdata->buffers+i);
    }
    free(renderdata->shaders);
    free(renderdata->objects);
    free(renderdata->cameras);
    free(renderdata->buffers);
}




// RASTERIZER

typedef struct {
    int column;
    renderdata_t *renderdata;
    int dataIndex;
    vertex_t *vertexBuffer;
} threaddata_t;


static pthread_mutex_t printf_mutex;

int sync_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    pthread_mutex_lock(&printf_mutex);
    vprintf(format, args);
    pthread_mutex_unlock(&printf_mutex);

    va_end(args);
}




void *execPixels(void *vargp) {

    threaddata_t data = *(threaddata_t*)vargp;

    renderdata_t *renderdata = data.renderdata;
    const int dataIndex = data.dataIndex;
    const int column = data.column;

    vertex_t *vertexBuffer = data.vertexBuffer;


    model_t *model = renderdata->objects[dataIndex];
    camera_t *camera = renderdata->cameras[dataIndex];
    shader_t *shader = renderdata->shaders[dataIndex];
    uniformbuffer_t *uniformbuffer = renderdata->buffers+dataIndex;

    vec_t *iplAttribs = NULL;
    if(model->nVertexAttribs > 0) {
        iplAttribs = calloc((size_t)model->nVertexAttribs, sizeof(vec_t));
    }

    bitmap_t *rendertarget = camera->rendertargets+0;
    const int rtWidth = rendertarget->width;
    const int rtHeight = rendertarget->height;

    const int columnWidth = rtWidth / N_THREADS;
    const int xStart = columnWidth * column;
    const int xEnd   = columnWidth * (column+1);

    for(int x=xStart; x<xEnd; x++) {
        for(int y=0; y<rtHeight; y++) {

            pixel_t *pixel = bmFastGetPixelAt(rendertarget, x, y);
            if(!pixel) {
                continue;
            }

            const int modelID = pixel->modelID;
            const int triangleID = pixel->triangleID;

            if(triangleID == -1 || modelID == -1 || modelID != model->modelID) {
                continue;
            }

            // get vertices
            vertex_t *v0 = vertexBuffer + (triangleID*3+0);
            vertex_t *v1 = vertexBuffer + (triangleID*3+1);
            vertex_t *v2 = vertexBuffer + (triangleID*3+2);

            vec_t pcBary = (vec_t){pixel->r, pixel->g, pixel->b, 0.0f};

            // interpolate vertex attributes
            vec_t iplUV, iplNrm, iplClr;

            interpolateBary(&iplUV, &v0->texCoord, &v1->texCoord, &v2->texCoord, &pcBary);
            interpolateBary(&iplNrm, &v0->normal, &v1->normal, &v2->normal, &pcBary);
            interpolateBary(&iplClr, &v0->color, &v1->color, &v2->color, &pcBary);

            if(v0->nAttribs > 0) {
                for(int i=0; i<v0->nAttribs; i++) {
                    interpolateBary(&iplAttribs[i], &v0->attribs[i], &v1->attribs[i], &v2->attribs[i], &pcBary);
                }
            }

            // call fragment shader
            shader->fsh(camera, model, shader, pixel, &iplUV, &iplNrm, &iplClr, iplAttribs, uniformbuffer);

        }
    }

    free(iplAttribs);

#ifdef USE_THREADS
    pthread_exit(NULL);
#endif

}




void processPixels(renderdata_t *renderdata, int dataIndex, vertex_t *vertexBuffer) {

    pthread_mutex_init(&printf_mutex, NULL);
    pthread_t thread_ids[N_THREADS];
    threaddata_t *threaddata;

    for(int i=0; i<N_THREADS; i++) {

        threaddata = malloc(sizeof(threaddata_t));
        threaddata->dataIndex = dataIndex;
        threaddata->renderdata = renderdata;
        threaddata->column = i;
        threaddata->vertexBuffer = vertexBuffer;


#ifndef USE_THREADS
        execPixels((void*)threaddata);
#else
        pthread_create(&thread_ids[i], NULL, execPixels, (void*)threaddata);
#endif

    }

#ifdef USE_THREADS
    for(int i=0; i<N_THREADS; i++) {
        pthread_join(thread_ids[i], NULL);
    }
#endif


}




void rasterizeTriangle(camera_t *camera, model_t *model, shader_t *shader, uniformbuffer_t *uniformbuffer, vertex_t *v0, vertex_t *v1, vertex_t *v2) {

    bitmap_t *rendertarget = camera->rendertargets+0;
    const int rtWidth = rendertarget->width;
    const int rtHeight = rendertarget->height;

    // calculate bounding
    float minY = v0->position.y;
    minY = min(minY, v1->position.y);
    minY = min(minY, v2->position.y);
    minY = min(minY, rtWidth-1);
    minY = max(minY, 0.0f);

    float maxY = v0->position.y;
    maxY = max(maxY, v1->position.y);
    maxY = max(maxY, v2->position.y);
    maxY = min(maxY, rtHeight-1);
    minY = max(minY, 0.0f);

    const int yStart = (int)floorf(minY);
    const int yEnd = (int)ceilf(maxY);

    // clear scanbuffer
    for(int i=yStart; i<yEnd; i++) {
        rendertarget->scanbufferMin[i] = rendertarget->width+100;
        rendertarget->scanbufferMax[i] = -(rendertarget->width+100);
    }

    // draw scanbuffer
    bhDrawLineToScanbuffer(rendertarget->scanbufferMin, rendertarget->scanbufferMax, rtHeight, (int)v0->position.x, (int)v0->position.y, (int)v1->position.x, (int)v1->position.y);
    bhDrawLineToScanbuffer(rendertarget->scanbufferMin, rendertarget->scanbufferMax, rtHeight, (int)v0->position.x, (int)v0->position.y, (int)v2->position.x, (int)v2->position.y);
    bhDrawLineToScanbuffer(rendertarget->scanbufferMin, rendertarget->scanbufferMax, rtHeight, (int)v1->position.x, (int)v1->position.y, (int)v2->position.x, (int)v2->position.y);

    // rasterize triangle
    static vec_t texelCoords;
    static vec_t baryCoords;
    static vec_t pcBary;

    const int yd = yEnd - yStart;
    for(int ys=0; ys<=yd; ys++) {
        const int y = ys+yStart;


        const int xStart = max(rendertarget->scanbufferMin[y]-1, 0);
        const int xEnd   = min(rendertarget->scanbufferMax[y]+1, rtWidth);

        const int xd = xEnd - xStart;
        for(int xs=0; xs<=xd; xs++) {
            const int x = xs+xStart;

            texelCoords = (vec_t){x+0.5f, y+0.5f, 0, 0};
            barycentric(&baryCoords, &v0->position, &v1->position, &v2->position, &texelCoords);

            // test if sample is part of triangle
            if(baryCoords.x < 0 || baryCoords.y < 0 || baryCoords.z < 0) {
                continue;
            }

            // get pixel
            pixel_t *pixel = bmFastGetPixelAt(rendertarget, x, y);
            if(!pixel) {
                continue;
            }

            // calc perspective correct bary coords
            const float oneOverW = baryCoords.x/v0->position.w + baryCoords.y/v1->position.w + baryCoords.z/v2->position.w;
            baryCorrectPerspective(&baryCoords, v0->position.w, v1->position.w, v2->position.w, oneOverW, &pcBary);

            // depth test
            vec_t iplPos;
            interpolateBary(&iplPos, &v0->position, &v1->position, &v2->position, &baryCoords);
            const float zPixel = pixel->z;
            if(zPixel < iplPos.z) {
                continue;
            }
            pixel->z = iplPos.z;
            pixel->modelID = model->modelID;
            pixel->triangleID = v0->triangleID;

            pixel->r = pcBary.x;
            pixel->g = pcBary.y;
            pixel->b = pcBary.z;

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
    dst->position   = src->position;
    dst->normal     = src->normal;
    dst->texCoord   = src->texCoord;
    dst->color      = src->color;
    dst->nAttribs   = src->nAttribs;
    memcpy(dst->attribs, src->attribs, sizeof(vec_t)*src->nAttribs);
    dst->triangleID = src->triangleID;
    dst->modelID    = src->modelID;
}




void rcDrawModel(renderdata_t *renderdata, int dataIndex) {

    sampleStart("drawModel");

    model_t *model = renderdata->objects[dataIndex];
    camera_t *camera = renderdata->cameras[dataIndex];
    shader_t *shader = renderdata->shaders[dataIndex];
    uniformbuffer_t *uniformbuffer = renderdata->buffers+dataIndex;

    bitmap_t *rendertarget = camera->rendertargets+0;
    float rtWidth = rendertarget->width;
    float rtHeight = rendertarget->height;

    matrix_t sst = camera->screenSpaceTransform;

    vertex_t *vertexBuffer = malloc(sizeof(vertex_t)*model->nTriangles*3);
    vec_t *attribBuffer = malloc(sizeof(vec_t)*model->nTriangles*3*model->nVertexAttribs);


    // call pre-shader
    shader->psh(camera, model, shader, uniformbuffer);

    // for each triangle
    const int nTris = model->nTriangles;
    for(int i=0, j=0; i<nTris; i++) {

        sampleStart("procVertex");

        triangle_t *triangle = model->triangles + i;

        // get vertices
        vertex_t *vo0 = triangle->vertices + 0;
        vertex_t *vo1 = triangle->vertices + 1;
        vertex_t *vo2 = triangle->vertices + 2;

        vertex_t *v0 = vertexBuffer + (j);
        v0->attribs = attribBuffer+(j*model->nVertexAttribs);
        j++;

        vertex_t *v1 = vertexBuffer + (j);
        v1->attribs = attribBuffer+(j*model->nVertexAttribs);
        j++;

        vertex_t *v2 = vertexBuffer + (j);
        v2->attribs = attribBuffer+(j*model->nVertexAttribs);
        j++;


        // copy values
        copyVertex(v0, vo0);
        copyVertex(v1, vo1);
        copyVertex(v2, vo2);

        // transform / project vertices
        shader->vsh(vo0, v0, shader, uniformbuffer);
        shader->vsh(vo1, v1, shader, uniformbuffer);
        shader->vsh(vo2, v2, shader, uniformbuffer);

        projectVertex(v0, v0, &sst);
        projectVertex(v1, v1, &sst);
        projectVertex(v2, v2, &sst);

        // cull depth
        if ((v0->position.z < 0 || v0->position.z > 1) || (v1->position.z < 0 || v1->position.z > 1) || (v2->position.z < 0 || v2->position.z > 1)) {
            sampleEnd("procVertex");
            continue;
        }

        // cull viewspace
        int nOutside = 0;
        if ((v0->position.x < 0 || v0->position.x > rtWidth) || (v0->position.y < 0 || v0->position.y > rtHeight)) { nOutside++; }
        if ((v1->position.x < 0 || v1->position.x > rtWidth) || (v1->position.y < 0 || v1->position.y > rtHeight)) { nOutside++; }
        if ((v2->position.x < 0 || v2->position.x > rtWidth) || (v2->position.y < 0 || v2->position.y > rtHeight)) { nOutside++; }
        if (nOutside == 3) {
            sampleEnd("procVertex");
            continue;
        }

        // cull backspace
        if (cullBackface(&v0->position, &v1->position, &v2->position)) {
            sampleEnd("procVertex");
            continue;
        }

        sampleEnd("procVertex");

        // rasterize
        sampleStart("rastTri");

        rasterizeTriangle(camera, model, shader, uniformbuffer, v0, v1, v2);

        sampleEnd("rastTri");

    }

    // process pixels
    processPixels(renderdata, dataIndex, vertexBuffer);

    free(attribBuffer);
    free(vertexBuffer);

    sampleEnd("drawModel");

}




void rcDrawRenderData(renderdata_t *renderdata) {
    for(int i=0; i<renderdata->nObjects; i++) {
        rcDrawModel(renderdata, i);
    }
}


