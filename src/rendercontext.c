#include "rendercontext.h"
#include "camera.h"
#include "bresenham.h"
#include "geometry.h"

#include <stdio.h>



void projectVertex(vertex_t *vertexOut, vertex_t *vertexIn, matrix_t *mvp, matrix_t *sst) {
    matTransform(&vertexOut->position, &vertexIn->position, mvp);
    matTransform(&vertexOut->position, &vertexOut->position, sst);
    vecPerspectiveDivide(&vertexOut->position, &vertexOut->position);
}



void rcDrawModelWireframe(camera_t *camera, model_t *model) {

    bitmap_t *rendertarget = camera->rendertargets;

    mdlUpdateTransform(model);
    matrix_t mvp; matMul(&mvp, &camera->viewProjection, &model->modelTransform);
    matrix_t sst = camera->screenSpaceTransform;

    vertex_t v0;
    vertex_t v1;
    vertex_t v2;

    for(int i=0; i<model->nTriangles; i++) {
        triangle_t *triangle = model->triangles+i;

        vertex_t *vo0 = triangle->vertices+0;
        vertex_t *vo1 = triangle->vertices+1;
        vertex_t *vo2 = triangle->vertices+2;

        projectVertex(&v0, vo0, &mvp, &sst);
        projectVertex(&v1, vo1, &mvp, &sst);
        projectVertex(&v2, vo2, &mvp, &sst);

        bhDrawLine(rendertarget, (int)v0.position.x, (int)v0.position.y, (int)v1.position.x, (int)v1.position.y, 1.0f, 1.0f, 1.0f);
        bhDrawLine(rendertarget, (int)v0.position.x, (int)v0.position.y, (int)v2.position.x, (int)v2.position.y, 1.0f, 1.0f, 1.0f);
        bhDrawLine(rendertarget, (int)v1.position.x, (int)v1.position.y, (int)v2.position.x, (int)v2.position.y, 1.0f, 1.0f, 1.0f);

    }


}