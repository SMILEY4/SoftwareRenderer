#include "model.h"
#include "objfile.h"
#include "bitmap.h"
#include <windows.h>
#include <math.h>



void mdlUpdateTransform(model_t *model) {
    matSetTranslation(&model->matTranslation, model->translation.x, model->translation.y, model->translation.z);
    matSetRotation(&model->matRotation, model->rotation.x, model->rotation.y, model->rotation.z);
    matSetScale(&model->matScale, model->scale.x, model->scale.y, model->scale.z);
    matMul3(&model->modelTransform, &model->matTranslation, &model->matRotation, &model->matScale);
}




void mdlCreateFromObj(obj_model_t *objmodel, model_t *model, char **textureFiles, unsigned int nTextures, unsigned int nVertexAttribs) {
    if(!objmodel) {
        return;
    }

    // reserve memory for triangles
    model->nTriangles = objmodel->nFaces;
    model->triangles = calloc(model->nTriangles, sizeof(triangle_t));


    // for each triangle
    for(int i=0; i<model->nTriangles; i++) {
        obj_face_t face = objmodel->faces[i];


        // get indices for vertices, texcoords, normals
        int iVert0 = face.vert_indices[0]-1;
        int iVert1 = face.vert_indices[1]-1;
        int iVert2 = face.vert_indices[2]-1;

        int iTex0 = face.tex_indices[0]-1;
        int iTex1 = face.tex_indices[1]-1;
        int iTex2 = face.tex_indices[2]-1;

        int iNorm0 = face.norm_Indices[0]-1;
        int iNorm1 = face.norm_Indices[1]-1;
        int iNorm2 = face.norm_Indices[2]-1;

        // set metadata of triangle
        triangle_t triangle;
        triangle.vertices = calloc(3, sizeof(vertex_t));

        // set positions for each vertex
        triangle.vertices[0].position = (vec_t) {objmodel->vertices[iVert0].x, objmodel->vertices[iVert0].y, objmodel->vertices[iVert0].z, 1.0};
        triangle.vertices[1].position = (vec_t) {objmodel->vertices[iVert1].x, objmodel->vertices[iVert1].y, objmodel->vertices[iVert1].z, 1.0};
        triangle.vertices[2].position = (vec_t) {objmodel->vertices[iVert2].x, objmodel->vertices[iVert2].y, objmodel->vertices[iVert2].z, 1.0};

        // set texcoords for each vertex
        if(objmodel->hasTexCoords) {
            triangle.vertices[0].texCoord = (vec_t) {objmodel->texcoords[iTex0].u, objmodel->texcoords[iTex0].v, objmodel->texcoords[iTex0].w, 1.0};
            triangle.vertices[1].texCoord = (vec_t) {objmodel->texcoords[iTex1].u, objmodel->texcoords[iTex1].v, objmodel->texcoords[iTex1].w, 1.0};
            triangle.vertices[2].texCoord = (vec_t) {objmodel->texcoords[iTex2].u, objmodel->texcoords[iTex2].v, objmodel->texcoords[iTex2].w, 1.0};
        }

        // set normals for each vertex
        if(objmodel->hasNormals) {
            triangle.vertices[0].normal = (vec_t) {objmodel->normals[iNorm0].x, objmodel->normals[iNorm0].y, objmodel->normals[iNorm0].z, 0.0};
            triangle.vertices[1].normal = (vec_t) {objmodel->normals[iNorm1].x, objmodel->normals[iNorm1].y, objmodel->normals[iNorm1].z, 0.0};
            triangle.vertices[2].normal = (vec_t) {objmodel->normals[iNorm2].x, objmodel->normals[iNorm2].y, objmodel->normals[iNorm2].z, 0.0};
        }

        // set color for each vertex
        triangle.vertices[0].color = (vec_t) {(float)rand()/(float)RAND_MAX, (float)rand()/(float)RAND_MAX, (float)rand()/(float)RAND_MAX, 1.0 };
        triangle.vertices[1].color = (vec_t) {(float)rand()/(float)RAND_MAX, (float)rand()/(float)RAND_MAX, (float)rand()/(float)RAND_MAX, 1.0 };
        triangle.vertices[2].color = (vec_t) {(float)rand()/(float)RAND_MAX, (float)rand()/(float)RAND_MAX, (float)rand()/(float)RAND_MAX, 1.0 };

        // set addition attribs for each vertex
        model->nVertexAttribs = nVertexAttribs;
        triangle.vertices[0].nAttribs = nVertexAttribs;
        triangle.vertices[1].nAttribs = nVertexAttribs;
        triangle.vertices[2].nAttribs = nVertexAttribs;
        if(nVertexAttribs > 0) {
            triangle.vertices[0].attribs = calloc(nVertexAttribs, sizeof(vec_t));
            triangle.vertices[1].attribs = calloc(nVertexAttribs, sizeof(vec_t));
            triangle.vertices[2].attribs = calloc(nVertexAttribs, sizeof(vec_t));
        } else {
            triangle.vertices[0].attribs = NULL;
            triangle.vertices[1].attribs = NULL;
            triangle.vertices[2].attribs = NULL;
        }

        triangle.triangleID = i;
        triangle.vertices[0].triangleID = i;
        triangle.vertices[1].triangleID = i;
        triangle.vertices[2].triangleID = i;
        model->triangles[i] = triangle;

    }



    // load texture
    model->nTextures = nTextures;
    if(textureFiles && nTextures > 0) {
        model->textures = calloc(nTextures, sizeof(bitmap_t));
        for(int i=0; i<nTextures; i++) {
            char *filename = textureFiles[i];
            bitmap_t texture;
            bmCreateFromPNG(&texture, filename);
            model->textures[i] = texture;
        }
    }


    // update transform matrix
    mdlUpdateTransform(model);
}




void mdlFreeModel(model_t *model) {
    if(model) {
        for(int i=0; i<model->nTriangles; i++) {
            triangle_t triangle = model->triangles[i];
            free(triangle.vertices);
        }
        for(int i=0; i<model->nTextures; i++) {
            bmDispose(&model->textures[i]);
        }
        free(model->triangles);
    }
}

