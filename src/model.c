#include "model.h"
#include "objfile.h"
#include "bitmap.h"
#include <windows.h>



void mdlCreateFromObj(obj_model_t *objmodel, model_t *model, char *fileTexture) {
    if(!objmodel) {
        return;
    }

    model->nTriangles = objmodel->nFaces;
    model->triangles = calloc(model->nTriangles, sizeof(triangle_t));

    for(int i=0; i<model->nTriangles; i++) {
        obj_face_t face = objmodel->faces[i];

        int iVert0 = face.vert_indices[0]-1;
        int iVert1 = face.vert_indices[1]-1;
        int iVert2 = face.vert_indices[2]-1;

        int iTex0 = face.tex_indices[0]-1;
        int iTex1 = face.tex_indices[1]-1;
        int iTex2 = face.tex_indices[2]-1;

        int iNorm0 = face.norm_Indices[0]-1;
        int iNorm1 = face.norm_Indices[1]-1;
        int iNorm2 = face.norm_Indices[2]-1;

        triangle_t triangle;

        triangle.vertices = calloc(3, sizeof(vertex_t));

        triangle.vertices[0].pos = (vec_t) {objmodel->vertices[iVert0].x, objmodel->vertices[iVert0].y, objmodel->vertices[iVert0].z, 1.0};
        triangle.vertices[1].pos = (vec_t) {objmodel->vertices[iVert1].x, objmodel->vertices[iVert1].y, objmodel->vertices[iVert1].z, 1.0};
        triangle.vertices[2].pos = (vec_t) {objmodel->vertices[iVert2].x, objmodel->vertices[iVert2].y, objmodel->vertices[iVert2].z, 1.0};

        if(objmodel->hasTexCoords) {
            triangle.vertices[0].texcoord = (vec_t) {objmodel->texcoords[iTex0].u, objmodel->texcoords[iTex0].v, objmodel->texcoords[iTex0].w, 1.0};
            triangle.vertices[1].texcoord = (vec_t) {objmodel->texcoords[iTex1].u, objmodel->texcoords[iTex1].v, objmodel->texcoords[iTex1].w, 1.0};
            triangle.vertices[2].texcoord = (vec_t) {objmodel->texcoords[iTex2].u, objmodel->texcoords[iTex2].v, objmodel->texcoords[iTex2].w, 1.0};
        }

        if(objmodel->hasNormals) {
            triangle.vertices[0].normal = (vec_t) {objmodel->normals[iNorm0].x, objmodel->normals[iNorm0].y, objmodel->normals[iNorm0].z, 0.0};
            triangle.vertices[1].normal = (vec_t) {objmodel->normals[iNorm1].x, objmodel->normals[iNorm1].y, objmodel->normals[iNorm1].z, 0.0};
            triangle.vertices[2].normal = (vec_t) {objmodel->normals[iNorm2].x, objmodel->normals[iNorm2].y, objmodel->normals[iNorm2].z, 0.0};
        }

        model->triangles[i] = triangle;

    }

    // load texture
    if(fileTexture) {
        bitmap_t texture;
        bmCreateFromPNG(&texture, fileTexture);
        model->texture = texture;
    }

}




void mdlFreeModel(model_t *model) {
    if(model) {
        free(model->triangles);
    }
}