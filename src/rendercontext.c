#include "rendercontext.h"
#include "bresenham.h"
#include "model.h"
#include "stopwatch.h"
#include "bitmap.h"
#include "geometry.h"
#include "input.h"
#include "camera.h"
#include "postprocess.h"
#include <math.h>
#include <stdio.h>
//#include <pthread.h>
#include <windows.h>

#define USE_THREADS
#define N_THREADS 8


void rcCreateRenderData(renderdata_t *renderdata, unsigned int nObjects, unsigned int ubMaxElements,
                        unsigned int ubMaxPointers, unsigned int nPPEffects) {

    renderdata->shaders = calloc((size_t) nObjects, sizeof(shader_t *));
    renderdata->objects = calloc((size_t) nObjects, sizeof(model_t *));
    renderdata->cameras = calloc((size_t) nObjects, sizeof(camera_t *));
    renderdata->buffers = calloc((size_t) nObjects, sizeof(uniformbuffer_t));
    for (int i = 0; i < nObjects; i++) {
        ubCreateBuffer(renderdata->buffers + i, ubMaxElements, ubMaxPointers);
    }
    renderdata->nObjects = nObjects;

    renderdata->ppEffects = calloc((size_t) nPPEffects, sizeof(postProcessEffect_t *));
    renderdata->nPPEffects = nPPEffects;
}


void rcFreeRenderData(renderdata_t *renderdata) {
    for (int i = 0; i < renderdata->nObjects; i++) {
        ubFreeBuffer(renderdata->buffers + i);
    }
    free(renderdata->shaders);
    free(renderdata->objects);
    free(renderdata->cameras);
    free(renderdata->buffers);
    free(renderdata->ppEffects);
}






// POST PROCESS

typedef struct {
    int column;
    renderdata_t *renderdata;
    bitmap_t *rendertarget;
} threaddata_postprocess_t;


DWORD WINAPI execPixelPostProcess(LPVOID vargp) {

    threaddata_postprocess_t data = *(threaddata_postprocess_t *) vargp;

    const int column = data.column;
    renderdata_t *renderdata = data.renderdata;
    bitmap_t *rendertarget = data.rendertarget;

    const int rtWidth = rendertarget->width;
    const int rtHeight = rendertarget->height;

    const int columnWidth = rtWidth / N_THREADS;
    const int xStart = columnWidth * column;
    const int xEnd = columnWidth * (column + 1);

    vec_t tmpColor = (vec_t) {0.0f, 0.0f, 0.0f, 0.0f};

    for (int x = xStart; x < xEnd; x++) {
        for (int y = 0; y < rtHeight; y++) {

            pixel_t *pixel = bmFastGetPixelAt(rendertarget, x, y);
            if (!pixel) {
                continue;
            }

            const int nEffects = renderdata->nPPEffects;
            for (int i = 0; i < nEffects; i++) {

                postProcessEffect_t *effect = renderdata->ppEffects[i];
                effect->ppFx(pixel, &tmpColor);

                pixel->r = tmpColor.r;
                pixel->g = tmpColor.g;
                pixel->b = tmpColor.b;
                pixel->a = tmpColor.a;

            }
        }
    }

    return 0;
}


void processPixelPostProcess(renderdata_t *renderdata, bitmap_t *rendertarget) {

    DWORD threadIds[N_THREADS];
    HANDLE threadArray[N_THREADS];
    threaddata_postprocess_t *dataArray[N_THREADS];

    for (int i = 0; i < N_THREADS; i++) {

        dataArray[i] = (threaddata_postprocess_t*) malloc(sizeof(threaddata_postprocess_t));
        dataArray[i]->column = i;
        dataArray[i]->rendertarget = rendertarget;
        dataArray[i]->renderdata = renderdata;

#ifndef USE_THREADS
        execPixelPostProcess((void *) threaddata);
#else
        threadArray[i] = CreateThread(NULL, 0, execPixelPostProcess, dataArray[i], 0, &threadIds[i]);
#endif

    }

#ifdef USE_THREADS
    WaitForMultipleObjects(N_THREADS, threadArray, TRUE, INFINITE);
    for (int i = 0; i < N_THREADS; i++) {
        CloseHandle(threadArray[i]);

    }
#endif


}




// RASTERIZER

typedef struct {
    int column;
    renderdata_t *renderdata;
    int dataIndex;
    vertex_t *vertexBuffer;
    int minX, maxX, minY, maxY;
} threaddata_shading_t;


DWORD WINAPI execPixelShading(LPVOID vargp) {

    threaddata_shading_t data = *(threaddata_shading_t *) vargp;

    renderdata_t *renderdata = data.renderdata;
    const int dataIndex = data.dataIndex;
    const int column = data.column;
    const int minY = data.minY;
    const int maxY = data.maxY;

    vertex_t *vertexBuffer = data.vertexBuffer;


    model_t *model = renderdata->objects[dataIndex];
    camera_t *camera = renderdata->cameras[dataIndex];
    shader_t *shader = renderdata->shaders[dataIndex];
    uniformbuffer_t *uniformbuffer = renderdata->buffers + dataIndex;

    vec_t *iplAttribs = NULL;
    if (model->nVertexAttribs > 0) {
        iplAttribs = calloc((size_t) model->nVertexAttribs, sizeof(vec_t));
    }

    bitmap_t *rendertarget = camera->rendertargets + 0;
    const int rtWidth = rendertarget->width;
    const int rtHeight = rendertarget->height;

    const int columnWidth = rtWidth / N_THREADS;
    const int xStart = columnWidth * column;
    const int xEnd = columnWidth * (column + 1);

    for (int x = xStart; x < xEnd; x++) {
        for (int y = minY; y <= maxY; y++) {

            pixel_t *pixel = bmFastGetPixelAt(rendertarget, x, y);
            if (!pixel) {
                continue;
            }

            const int modelID = pixel->modelID;
            const int triangleID = pixel->triangleID;

            if (triangleID == -1 || modelID == -1 || modelID != model->modelID) {
                continue;
            }

            // get vertices
            vertex_t *v0 = vertexBuffer + (triangleID * 3 + 0);
            vertex_t *v1 = vertexBuffer + (triangleID * 3 + 1);
            vertex_t *v2 = vertexBuffer + (triangleID * 3 + 2);

            vec_t pcBary = (vec_t) {pixel->r, pixel->g, pixel->b, 0.0f};

            // interpolate vertex attributes
            vec_t iplUV, iplNrm, iplClr;

            interpolateBary(&iplUV, &v0->texCoord, &v1->texCoord, &v2->texCoord, &pcBary);
            interpolateBary(&iplNrm, &v0->normal, &v1->normal, &v2->normal, &pcBary);
            interpolateBary(&iplClr, &v0->color, &v1->color, &v2->color, &pcBary);

            if (v0->nAttribs > 0) {
                for (int i = 0; i < v0->nAttribs; i++) {
                    interpolateBary(&iplAttribs[i], &v0->attribs[i], &v1->attribs[i], &v2->attribs[i], &pcBary);
                }
            }

            // call fragment shader
            shader->fsh(camera, model, shader, pixel, &iplUV, &iplNrm, &iplClr, iplAttribs, uniformbuffer);

        }
    }

    free(iplAttribs);

    return 0;

}


void processPixelShading(renderdata_t *renderdata, int dataIndex, vertex_t *vertexBuffer, int minX, int minY, int maxX,
                         int maxY) {

    DWORD threadIds[N_THREADS];
    HANDLE threadArray[N_THREADS];
    threaddata_shading_t *dataArray[N_THREADS];

    for (int i = 0; i < N_THREADS; i++) {

        dataArray[i] = (threaddata_shading_t *) malloc(sizeof(threaddata_shading_t));
        dataArray[i]->dataIndex = dataIndex;
        dataArray[i]->renderdata = renderdata;
        dataArray[i]->column = i;
        dataArray[i]->vertexBuffer = vertexBuffer;
        dataArray[i]->minX = minX;
        dataArray[i]->minY = minY;
        dataArray[i]->maxX = maxX;
        dataArray[i]->maxY = maxY;

#ifndef USE_THREADS
        execPixelShading((void*)threaddata);
#else
        threadArray[i] = CreateThread(NULL, 0, execPixelShading, dataArray[i], 0, &threadIds[i]);
#endif

    }

#ifdef USE_THREADS
    WaitForMultipleObjects(N_THREADS, threadArray, TRUE, INFINITE);
    for (int i = 0; i < N_THREADS; i++) {
        CloseHandle(threadArray[i]);

    }
#endif


}


void rasterizeTriangle(camera_t *camera, model_t *model, shader_t *shader, uniformbuffer_t *uniformbuffer, vertex_t *v0,
                       vertex_t *v1, vertex_t *v2) {

    bitmap_t *rendertarget = camera->rendertargets + 0;
    const int rtWidth = rendertarget->width;
    const int rtHeight = rendertarget->height;

    // calculate bounding
    float minY = v0->position.y;
    minY = min(minY, v1->position.y);
    minY = min(minY, v2->position.y);
    minY = min(minY, rtWidth - 1);
    minY = max(minY, 0.0f);

    float maxY = v0->position.y;
    maxY = max(maxY, v1->position.y);
    maxY = max(maxY, v2->position.y);
    maxY = min(maxY, rtHeight - 1);
    minY = max(minY, 0.0f);

    const int yStart = (int) floorf(minY);
    const int yEnd = (int) ceilf(maxY);

    // clear scanbuffer
    for (int i = yStart; i < yEnd; i++) {
        rendertarget->scanbufferMin[i] = rendertarget->width + 100;
        rendertarget->scanbufferMax[i] = -(rendertarget->width + 100);
    }

    // draw scanbuffer
    bhDrawLineToScanbuffer(rendertarget->scanbufferMin, rendertarget->scanbufferMax, rtHeight, (int) v0->position.x,
                           (int) v0->position.y, (int) v1->position.x, (int) v1->position.y);
    bhDrawLineToScanbuffer(rendertarget->scanbufferMin, rendertarget->scanbufferMax, rtHeight, (int) v0->position.x,
                           (int) v0->position.y, (int) v2->position.x, (int) v2->position.y);
    bhDrawLineToScanbuffer(rendertarget->scanbufferMin, rendertarget->scanbufferMax, rtHeight, (int) v1->position.x,
                           (int) v1->position.y, (int) v2->position.x, (int) v2->position.y);

    // rasterize triangle
    static vec_t texelCoords;
    static vec_t baryCoords;
    static vec_t pcBary;

    const int yd = yEnd - yStart;
    for (int ys = 0; ys <= yd; ys++) {
        const int y = ys + yStart;


        const int xStart = max(rendertarget->scanbufferMin[y] - 1, 0);
        const int xEnd = min(rendertarget->scanbufferMax[y] + 1, rtWidth);

        const int xd = xEnd - xStart;
        for (int xs = 0; xs <= xd; xs++) {
            const int x = xs + xStart;

            texelCoords = (vec_t) {x + 0.5f, y + 0.5f, 0, 0};
            barycentric(&baryCoords, &v0->position, &v1->position, &v2->position, &texelCoords);

            // test if sample is part of triangle
            if (baryCoords.x < 0 || baryCoords.y < 0 || baryCoords.z < 0) {
                continue;
            }

            // get pixel
            pixel_t *pixel = bmFastGetPixelAt(rendertarget, x, y);
            if (!pixel) {
                continue;
            }

            // calc perspective correct bary coords
            const float oneOverW =
                    baryCoords.x / v0->position.w + baryCoords.y / v1->position.w + baryCoords.z / v2->position.w;
            baryCorrectPerspective(&baryCoords, v0->position.w, v1->position.w, v2->position.w, oneOverW, &pcBary);

            // depth test
            vec_t iplPos;
            interpolateBary(&iplPos, &v0->position, &v1->position, &v2->position, &baryCoords);
            const float zPixel = pixel->z;
            if (zPixel < iplPos.z) {
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
    float d = vecDot(&triNormal, &(vec_t) {0.0f, 0.0f, 1.0f, 0.0f});
    if (d < 0) {
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
    memcpy(dst->attribs, src->attribs, sizeof(vec_t) * src->nAttribs);
    dst->triangleID = src->triangleID;
    dst->modelID = src->modelID;
}


void rcDrawModel(renderdata_t *renderdata, int dataIndex) {

    model_t *model = renderdata->objects[dataIndex];
    camera_t *camera = renderdata->cameras[dataIndex];
    shader_t *shader = renderdata->shaders[dataIndex];
    uniformbuffer_t *uniformbuffer = renderdata->buffers + dataIndex;

    bitmap_t *rendertarget = camera->rendertargets + 0;
    float rtWidth = rendertarget->width;
    float rtHeight = rendertarget->height;

    matrix_t sst = camera->screenSpaceTransform;

    vertex_t *vertexBuffer = malloc(sizeof(vertex_t) * model->nTriangles * 3);
    vec_t *attribBuffer = malloc(sizeof(vec_t) * model->nTriangles * 3 * model->nVertexAttribs);


    // call pre-shader
    shader->psh(camera, model, shader, uniformbuffer);

    int minX = (int) rtWidth;
    int minY = (int) rtHeight;
    int maxX = 0;
    int maxY = 0;

    // for each triangle
    const int nTris = model->nTriangles;
    for (int i = 0, j = 0; i < nTris; i++) {

        triangle_t *triangle = model->triangles + i;

        // get vertices
        vertex_t *vo0 = triangle->vertices + 0;
        vertex_t *vo1 = triangle->vertices + 1;
        vertex_t *vo2 = triangle->vertices + 2;

        vertex_t *v0 = vertexBuffer + (j);
        v0->attribs = attribBuffer + (j * model->nVertexAttribs);
        j++;

        vertex_t *v1 = vertexBuffer + (j);
        v1->attribs = attribBuffer + (j * model->nVertexAttribs);
        j++;

        vertex_t *v2 = vertexBuffer + (j);
        v2->attribs = attribBuffer + (j * model->nVertexAttribs);
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
        if ((v0->position.z < 0 || v0->position.z > 1) || (v1->position.z < 0 || v1->position.z > 1) ||
            (v2->position.z < 0 || v2->position.z > 1)) {
            continue;
        }

        // cull viewspace
        int nOutside = 0;
        if ((v0->position.x < 0 || v0->position.x > rtWidth) ||
            (v0->position.y < 0 || v0->position.y > rtHeight)) { nOutside++; }
        if ((v1->position.x < 0 || v1->position.x > rtWidth) ||
            (v1->position.y < 0 || v1->position.y > rtHeight)) { nOutside++; }
        if ((v2->position.x < 0 || v2->position.x > rtWidth) ||
            (v2->position.y < 0 || v2->position.y > rtHeight)) { nOutside++; }
        if (nOutside == 3) {
            continue;
        }

        // cull backspace
        if (cullBackface(&v0->position, &v1->position, &v2->position)) {
            continue;
        }


        // rasterize
        minX = min(minX, (int) floorf(v0->position.x));
        minY = min(minY, (int) floorf(v0->position.y));
        maxX = max(maxX, (int) ceilf(v0->position.x));
        maxY = max(maxY, (int) ceilf(v0->position.y));
        minX = min(minX, (int) floorf(v1->position.x));
        minY = min(minY, (int) floorf(v1->position.y));
        maxX = max(maxX, (int) ceilf(v1->position.x));
        maxY = max(maxY, (int) ceilf(v1->position.y));
        minX = min(minX, (int) floorf(v2->position.x));
        minY = min(minY, (int) floorf(v2->position.y));
        maxX = max(maxX, (int) ceilf(v2->position.x));
        maxY = max(maxY, (int) ceilf(v2->position.y));

        rasterizeTriangle(camera, model, shader, uniformbuffer, v0, v1, v2);

    }

    // process pixels
    minX = max(0, min(minX, (int) rtWidth - 1));
    minY = max(0, min(minY, (int) rtHeight - 1));
    maxX = max(0, min(maxX, (int) rtWidth - 1));
    maxY = max(0, min(maxY, (int) rtHeight - 1));
    processPixelShading(renderdata, dataIndex, vertexBuffer, minX, minY, maxX, maxY);

    free(attribBuffer);
    free(vertexBuffer);

    // post process effects
    processPixelPostProcess(renderdata, renderdata->cameras[0]->rendertargets + 0);

}


void rcDrawWireframeModel(renderdata_t *renderdata, int dataIndex) {


    model_t *model = renderdata->objects[dataIndex];
    camera_t *camera = renderdata->cameras[dataIndex];
    shader_t *shader = renderdata->shaders[dataIndex];
    uniformbuffer_t *uniformbuffer = renderdata->buffers + dataIndex;

    bitmap_t *rendertarget = camera->rendertargets + 0;
    float rtWidth = rendertarget->width;
    float rtHeight = rendertarget->height;

    matrix_t sst = camera->screenSpaceTransform;

    vertex_t *vertexBuffer = malloc(sizeof(vertex_t) * model->nTriangles * 3);
    vec_t *attribBuffer = malloc(sizeof(vec_t) * model->nTriangles * 3 * model->nVertexAttribs);


    // call pre-shader
    shader->psh(camera, model, shader, uniformbuffer);


    // for each triangle
    const int nTris = model->nTriangles;
    for (int i = 0, j = 0; i < nTris; i++) {

        triangle_t *triangle = model->triangles + i;

        // get vertices
        vertex_t *vo0 = triangle->vertices + 0;
        vertex_t *vo1 = triangle->vertices + 1;
        vertex_t *vo2 = triangle->vertices + 2;

        vertex_t *v0 = vertexBuffer + (j);
        v0->attribs = attribBuffer + (j * model->nVertexAttribs);
        j++;

        vertex_t *v1 = vertexBuffer + (j);
        v1->attribs = attribBuffer + (j * model->nVertexAttribs);
        j++;

        vertex_t *v2 = vertexBuffer + (j);
        v2->attribs = attribBuffer + (j * model->nVertexAttribs);
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
        if ((v0->position.z < 0 || v0->position.z > 1) || (v1->position.z < 0 || v1->position.z > 1) ||
            (v2->position.z < 0 || v2->position.z > 1)) {
            continue;
        }

        // cull viewspace
        int nOutside = 0;
        if ((v0->position.x < 0 || v0->position.x > rtWidth) ||
            (v0->position.y < 0 || v0->position.y > rtHeight)) { nOutside++; }
        if ((v1->position.x < 0 || v1->position.x > rtWidth) ||
            (v1->position.y < 0 || v1->position.y > rtHeight)) { nOutside++; }
        if ((v2->position.x < 0 || v2->position.x > rtWidth) ||
            (v2->position.y < 0 || v2->position.y > rtHeight)) { nOutside++; }
        if (nOutside == 3) {
            continue;
        }

        // cull backspace
        if (cullBackface(&v0->position, &v1->position, &v2->position)) {
            continue;
        }


        // rasterize
        bhDrawLine3D(rendertarget, (int) v0->position.x, (int) v0->position.y, v0->position.z, (int) v1->position.x,
                     (int) v1->position.y, v1->position.z, 1.0f, 0.2f, 0.2f);
        bhDrawLine3D(rendertarget, (int) v0->position.x, (int) v0->position.y, v0->position.z, (int) v2->position.x,
                     (int) v2->position.y, v2->position.z, 1.0f, 0.2f, 0.2f);
        bhDrawLine3D(rendertarget, (int) v1->position.x, (int) v1->position.y, v1->position.z, (int) v2->position.x,
                     (int) v2->position.y, v2->position.z, 1.0f, 0.2f, 0.2f);

    }


    free(attribBuffer);
    free(vertexBuffer);


}


void rcDrawRenderData(renderdata_t *renderdata, int drawAsWireframe) {
    if (drawAsWireframe) {
        for (int i = 0; i < renderdata->nObjects; i++) {
            rcDrawWireframeModel(renderdata, i);
        }
    } else {
        for (int i = 0; i < renderdata->nObjects; i++) {
            rcDrawModel(renderdata, i);
        }
    }
}


