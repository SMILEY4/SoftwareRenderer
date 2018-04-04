#include "shader.h"
#include "model.h"
#include "bitmap.h"
#include "camera.h"
#include "geometry.h"
#include "shaderutils.h"
#include "stopwatch.h"
#include <math.h>
#include <stdio.h>

vec_t poissonDisk[64];


// UTILS

void shInit() {
    poissonDisk[0]  = (vec_t){-0.613392f, 0.617481f, 0.0f, 0.0f};
    poissonDisk[1]  = (vec_t){ 0.170019f, -0.040254f,  0.0f,  0.0f};
    poissonDisk[2]  = (vec_t){-0.299417f,  0.791925f,  0.0f,  0.0f};
    poissonDisk[3]  = (vec_t){ 0.645680f,  0.493210f,  0.0f,  0.0f};
    poissonDisk[4]  = (vec_t){-0.651784f,  0.717887f,  0.0f,  0.0f};
    poissonDisk[5]  = (vec_t){ 0.421003f,  0.027070f,  0.0f,  0.0f};
    poissonDisk[6]  = (vec_t){-0.817194f, -0.271096f,  0.0f,  0.0f};
    poissonDisk[7]  = (vec_t){-0.705374f, -0.668203f,  0.0f,  0.0f};
    poissonDisk[8]  = (vec_t){ 0.977050f, -0.108615f,  0.0f,  0.0f};
    poissonDisk[9]  = (vec_t){ 0.063326f,  0.142369f,  0.0f,  0.0f};
    poissonDisk[10] = (vec_t){ 0.203528f,  0.214331f,  0.0f,  0.0f};
    poissonDisk[11] = (vec_t){-0.667531f,  0.326090f,  0.0f,  0.0f};
    poissonDisk[12] = (vec_t){-0.098422f, -0.295755f,  0.0f,  0.0f};
    poissonDisk[13] = (vec_t){-0.885922f,  0.215369f,  0.0f,  0.0f};
    poissonDisk[14] = (vec_t){ 0.566637f,  0.605213f,  0.0f,  0.0f};
    poissonDisk[15] = (vec_t){ 0.039766f, -0.396100f,  0.0f,  0.0f};
    poissonDisk[16] = (vec_t){ 0.751946f,  0.453352f,  0.0f,  0.0f};
    poissonDisk[17] = (vec_t){ 0.078707f, -0.715323f,  0.0f,  0.0f};
    poissonDisk[18] = (vec_t){-0.075838f, -0.529344f,  0.0f,  0.0f};
    poissonDisk[19] = (vec_t){ 0.724479f, -0.580798f,  0.0f,  0.0f};
    poissonDisk[20] = (vec_t){ 0.222999f, -0.215125f,  0.0f,  0.0f};
    poissonDisk[21] = (vec_t){-0.467574f, -0.405438f,  0.0f,  0.0f};
    poissonDisk[22] = (vec_t){-0.248268f, -0.814753f,  0.0f,  0.0f};
    poissonDisk[23] = (vec_t){ 0.354411f, -0.887570f,  0.0f,  0.0f};
    poissonDisk[24] = (vec_t){ 0.175817f,  0.382366f,  0.0f,  0.0f};
    poissonDisk[25] = (vec_t){ 0.487472f, -0.063082f,  0.0f,  0.0f};
    poissonDisk[26] = (vec_t){-0.084078f,  0.898312f,  0.0f,  0.0f};
    poissonDisk[27] = (vec_t){ 0.488876f, -0.783441f,  0.0f,  0.0f};
    poissonDisk[28] = (vec_t){ 0.470016f,  0.217933f,  0.0f,  0.0f};
    poissonDisk[29] = (vec_t){-0.696890f, -0.549791f,  0.0f,  0.0f};
    poissonDisk[30] = (vec_t){-0.149693f,  0.605762f,  0.0f,  0.0f};
    poissonDisk[31] = (vec_t){ 0.034211f,  0.979980f,  0.0f,  0.0f};
    poissonDisk[32] = (vec_t){ 0.503098f, -0.308878f,  0.0f,  0.0f};
    poissonDisk[33] = (vec_t){-0.016205f, -0.872921f,  0.0f,  0.0f};
    poissonDisk[34] = (vec_t){ 0.385784f, -0.393902f,  0.0f,  0.0f};
    poissonDisk[35] = (vec_t){-0.146886f, -0.859249f,  0.0f,  0.0f};
    poissonDisk[36] = (vec_t){ 0.643361f,  0.164098f,  0.0f,  0.0f};
    poissonDisk[37] = (vec_t){ 0.634388f, -0.049471f,  0.0f,  0.0f};
    poissonDisk[38] = (vec_t){-0.688894f,  0.007843f,  0.0f,  0.0f};
    poissonDisk[39] = (vec_t){ 0.464034f, -0.188818f,  0.0f,  0.0f};
    poissonDisk[40] = (vec_t){-0.440840f,  0.137486f,  0.0f,  0.0f};
    poissonDisk[41] = (vec_t){ 0.364483f,  0.511704f,  0.0f,  0.0f};
    poissonDisk[42] = (vec_t){ 0.034028f,  0.325968f,  0.0f,  0.0f};
    poissonDisk[43] = (vec_t){ 0.099094f, -0.308023f,  0.0f,  0.0f};
    poissonDisk[44] = (vec_t){ 0.693960f, -0.366253f,  0.0f,  0.0f};
    poissonDisk[45] = (vec_t){ 0.678884f, -0.204688f,  0.0f,  0.0f};
    poissonDisk[46] = (vec_t){ 0.001801f,  0.780328f,  0.0f,  0.0f};
    poissonDisk[47] = (vec_t){ 0.145177f, -0.898984f,  0.0f,  0.0f};
    poissonDisk[48] = (vec_t){ 0.062655f, -0.611866f,  0.0f,  0.0f};
    poissonDisk[49] = (vec_t){ 0.315226f, -0.604297f,  0.0f,  0.0f};
    poissonDisk[50] = (vec_t){-0.780145f,  0.486251f,  0.0f,  0.0f};
    poissonDisk[51] = (vec_t){-0.371868f,  0.882138f,  0.0f,  0.0f};
    poissonDisk[52] = (vec_t){ 0.200476f,  0.494430f,  0.0f,  0.0f};
    poissonDisk[53] = (vec_t){-0.494552f, -0.711051f,  0.0f,  0.0f};
    poissonDisk[54] = (vec_t){ 0.612476f,  0.705252f,  0.0f,  0.0f};
    poissonDisk[55] = (vec_t){-0.578845f, -0.768792f,  0.0f,  0.0f};
    poissonDisk[56] = (vec_t){-0.772454f, -0.090976f,  0.0f,  0.0f};
    poissonDisk[57] = (vec_t){ 0.504440f,  0.372295f,  0.0f,  0.0f};
    poissonDisk[58] = (vec_t){ 0.155736f,  0.065157f,  0.0f,  0.0f};
    poissonDisk[59] = (vec_t){ 0.391522f,  0.849605f,  0.0f,  0.0f};
    poissonDisk[60] = (vec_t){-0.620106f, -0.328104f,  0.0f,  0.0f};
    poissonDisk[61] = (vec_t){ 0.789239f, -0.419965f,  0.0f,  0.0f};
    poissonDisk[62] = (vec_t){-0.545396f,  0.538133f,  0.0f,  0.0f};
    poissonDisk[63] = (vec_t){-0.178564f, -0.596057f,  0.0f,  0.0f};
}








// SHADOW PASS

/*
 * UNIFORM BUFFER LAYOUT
 * u0: mvp
 * */

void pshShadowPass(camera_t *camera, model_t *model, shader_t *shader, uniformbuffer_t *uniformbuffer) {
    /*
     * - calculates mvp for object
     * - write mvp in uniformbuffer (u0)
     * */
    mdlUpdateTransform(model);
    matrix_t mvp;
    matMul(&mvp, &camera->viewProjection, &model->modelTransform);
    ubSetUniform(uniformbuffer, 0, &mvp, sizeof(matrix_t));

}




void vshShadowPass(vertex_t *vertexIn, vertex_t *vertexOut, shader_t *shader, uniformbuffer_t *uniformbuffer) {
    /*
     * - read mvp from uniformbuffer (u0)
     * - transform vertex
     * */
    matrix_t *mvp = (matrix_t*)ubGetUniform(uniformbuffer, 0);
    matTransform(&vertexOut->position, &vertexIn->position, mvp);
}




void fshShadowPass(camera_t *camera, model_t *model, shader_t *shader, pixel_t *pixel, vec_t *iplUV, vec_t *iplNrm, vec_t *iplClr, vec_t *iplAttribs, uniformbuffer_t *uniformbuffer) {
    /*
     * set pixel color to fragment depth
     * */
    float d = reverseDepth(pixel->z, camera->zNear, camera->zFar) / camera->zFar;
    pixel->r = pixel->z;
    pixel->g = d;
    pixel->b = d;
    pixel->a = 1.0f;
}







// MAIN PASS

/*
 * UNIFORM BUFFER LAYOUT
 * u0: mvp
 * u1: mvp_shadow
 * u2: mdlTransform
 * p0: camera_shadow
 * p1: skybox
 * */


void pshDefault(camera_t *camera, model_t *model, shader_t *shader, uniformbuffer_t *uniformbuffer) {

    /*
     * - calculates mvp for object
     * - write mvp in uniformbuffer (u0)
     *
     * - write mdlTransform in uniformbuffer (u2)
     *
     * - read camera_shadow from uniformbuffer (p0)
     * - write mvp_shadow in uniformbuffer (u1)
     * */

    mdlUpdateTransform(model);

    matrix_t mvp;
    matMul(&mvp, &camera->viewProjection, &model->modelTransform);
    ubSetUniform(uniformbuffer, 0, &mvp, sizeof(matrix_t));

    ubSetUniform(uniformbuffer, 2, &model->modelTransform, sizeof(matrix_t));

    camera_t *cameraShadow = (camera_t*)ubGetPoiner(uniformbuffer, 0);
    matrix_t mvpShadow;
    matMul(&mvpShadow, &cameraShadow->viewProjection, &model->modelTransform);
    ubSetUniform(uniformbuffer, 1, &mvpShadow, sizeof(matrix_t));

    srand(2342);

}




void vshDefault(vertex_t *vertexIn, vertex_t *vertexOut, shader_t *shader, uniformbuffer_t *uniformbuffer) {

    /*
     * - read mvp from uniformbuffer (u0)
     * - read mdlTrans from uniformbuffer (u2)
     *
     * - transform vertex
     *
     * - transform normals
     *
     * - read camera_shadow from uniformbuffer (p0)
     * - read mvp_shadow from uniformbuffer (u1)
     *
     * */

    matrix_t *mvp = (matrix_t*)ubGetUniform(uniformbuffer, 0);
    matrix_t *mdlTransform = (matrix_t*)ubGetUniform(uniformbuffer, 2);

    // vertex
    matTransform(&vertexOut->position, &vertexIn->position, mvp);
    matTransform(&vertexOut->attribs[1], &vertexIn->position, mdlTransform);

    // normal
    matTransform(&vertexOut->normal, &vertexIn->normal, mdlTransform);

    // vertex (shadow)
    camera_t *cameraShadow = (camera_t*)ubGetPoiner(uniformbuffer, 0);
    matrix_t *mvpShadow = (matrix_t*)ubGetUniform(uniformbuffer, 1);

    matTransform(&vertexOut->attribs[0], &vertexIn->position, mvpShadow);
    matTransform(&vertexOut->attribs[0], &vertexOut->attribs[0], &cameraShadow->screenSpaceTransform);
    vecPerspectiveDivide(&vertexOut->attribs[0], &vertexOut->attribs[0]);
}





// Specular BRDF Functions






void fshDefault_diablo(camera_t *camera, model_t *model, shader_t *shader, pixel_t *pixel, vec_t *iplUV, vec_t *iplNrm, vec_t *iplClr, vec_t *iplAttribs, uniformbuffer_t *uniformbuffer) {

    // get data
    vec_t iplPosShadow = iplAttribs[0];
    vec_t iplWorldPos = iplAttribs[1];
    camera_t *cameraShadow = (camera_t*)ubGetPoiner(uniformbuffer, 0);
    bitmap_t *shadowmap = cameraShadow->rendertargets+0;
    bitmap_t *envmap = (bitmap_t*)ubGetPoiner(uniformbuffer, 1);


    // calculate light dir  (world pos -> light pow)
    vec_t L, lightDir;
    vec_t lightPos = (vec_t){cameraShadow->pos.x, cameraShadow->pos.y, cameraShadow->pos.z, 0.0f};
    vecSub(&lightDir, &lightPos, &iplWorldPos);
    vecNormalize(&L, &lightDir);


    // calculate view dir
    vec_t V, viewDir;
    vecSub(&viewDir, &camera->pos, &iplWorldPos);
    vecNormalize(&V, &viewDir);


    // calculate normal (+normal mapping)
    pixel_t *pxNMTangent = bmGetPixelUV(model->textures+2, iplUV->x, iplUV->y, 1);
    vec_t Nm = (vec_t){pxNMTangent->r*2.0f-1.0f, pxNMTangent->g*2.0f-1.0f, pxNMTangent->b*2.0f-1.0f, 0.0f};

    vec_t Nv  = *iplNrm;
    vec_t Nv2 = {Nv.x+4, Nv.y+4, Nv.z+4};
    vec_t Tv;   vecCross(&Tv, &Nv, &Nv2);
    vec_t Bv;   vecCross(&Bv, &Nv, &Tv);

    Nv.w = 0.0; vecNormalize(&Nv, &Nv);
    Tv.w = 0.0; vecNormalize(&Tv, &Tv);
    Bv.w = 0.0; vecNormalize(&Bv, &Bv);

    matrix_t TBN;
    matrix_t TBN0;
    TBN.m[0][0] = Tv.x;    TBN.m[0][1] = Tv.y;    TBN.m[0][2] = Tv.z;    TBN.m[0][3] = 0.0f;
    TBN.m[1][0] = Bv.x;    TBN.m[1][1] = Bv.y;    TBN.m[1][2] = Bv.z;    TBN.m[1][3] = 0.0f;
    TBN.m[2][0] = Nv.x;    TBN.m[2][1] = Nv.y;    TBN.m[2][2] = Nv.z;    TBN.m[2][3] = 0.0f;
    TBN.m[3][0] = 0.0f;    TBN.m[3][1] = 0.0f;    TBN.m[3][2] = 0.0f;    TBN.m[3][3] = 1.0f;
    matTranspose(&TBN0, &TBN);

    vec_t N;
    matTransform(&N, &Nm, &TBN0);
    vecNormalize(&N, &N);



    // calc reflection dir
    vec_t R;
    vecReflect(&R, &V, &N);
    R.w = 0.0;
    vecNormalize(&R, &R);


    // calculate shadow
    float visibility = 0.0f;
    float cosTheta = vecDot(&N, &L);


    if(cosTheta < 0) {
        visibility = 0.0f;

    } else {
        float bias = max(1.2f * (1.0f - cosTheta), 0.05f);
        float radius = 1.5f;

        int nOccluded = 0;
        int nSamples = 0;

        for(int i=0; i<20; i++) {

            vec_t samplePos;
            int pdIndex = (int)(randFloat()*63);
            samplePos.x = iplPosShadow.x + poissonDisk[pdIndex].x*radius;
            samplePos.y = iplPosShadow.y + poissonDisk[pdIndex].y*radius;
            pixel_t *shadowPX = bmGetPixelAt(shadowmap, (unsigned int)samplePos.x, (unsigned int)samplePos.y, 1);


            float zShadow = reverseDepth(iplPosShadow.z, cameraShadow->zNear, cameraShadow->zFar);
            float zPixel = reverseDepth(shadowPX->r, cameraShadow->zNear, cameraShadow->zFar);

            if(zPixel < zShadow-bias) {
                nOccluded++;
            }

            nSamples++;

            // early out
            if( i==6) {
                if(nOccluded >= 6) {
                    break;
                }
                if(nOccluded == 0) {
                    break;
                }
            }

        }

        visibility = 1.0f - ((float)nOccluded / (float)nSamples);

    }


    vec_t ambient = {0.3f, 0.3f, 0.4f, 0.0f};
    vec_t lightColor = {1.6f, 1.1f, 0.8f, 1.2f};
    vecScale(&lightColor, &lightColor, lightColor.w);


    // get material information
    pixel_t *pxDiffuse  = bmGetPixelUV(model->textures+0, iplUV->x, iplUV->y, 1);
    pixel_t *pxSpec     = bmGetPixelUV(model->textures+3, iplUV->x, iplUV->y, 1);
    pixel_t *pxGlow     = bmGetPixelUV(model->textures+4, iplUV->x, iplUV->y, 1);

    vec_t baseColor = (vec_t){pxDiffuse->r, pxDiffuse->g, pxDiffuse->b};
    vec_t ssColor = (vec_t){0.0f, 0.0f, 0.0f, 0.0f};
    const float metalness = 0.0;
    float roughness = 1.0f - ( (pxSpec->r+pxSpec->g+pxSpec->b) / 3.0f);
    roughness = pow5(roughness) * 0.45f + 0.2f;

    // calculate shading
    vec_t colorDefaultLight; shadeDefaultLight(&colorDefaultLight, &N, &V, &L, &lightColor, &baseColor, &baseColor, &ssColor, metalness, roughness, 1.0f);
    vecScale(&colorDefaultLight, &colorDefaultLight, visibility);

    vec_t colorEnv; shadeEnvironment(&colorEnv, envmap, &baseColor, &N);
    vecScale(&colorEnv, &colorEnv, 0.4f);

    vec_t colorShading = (vec_t){0,0,0,0};
    vecAdd(&colorShading, &colorShading, &colorDefaultLight);
    vecAdd(&colorShading, &colorShading, &colorEnv);


    pixel->r = limit(colorShading.r);
    pixel->g = limit(colorShading.g);
    pixel->b = limit(colorShading.b);
    pixel->a = 1.0f;



}

























