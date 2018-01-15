#include "objfile.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>




bool firstPass(FILE *fp, obj_model_t *model) {
    if(!fp || !model) {
        return false;
    }

    int nVertices = 0;
    int nTexCoords = 0;
    int nNormals = 0;
    int nFaces = 0;

    char buf[256];
    while(fgets(buf, sizeof(buf), fp) != NULL) {

        if(buf[0] == 'v') {
            if (buf[1] == ' ') {
                nVertices++;
            } else if (buf[1] == 't') {
                nTexCoords++;
            } else if (buf[1] == 'n') {
                nNormals++;
            }
        } else if(buf[0] == 'f') {
            nFaces++;
        } else {
            // unknown -> ignore
        }

    }

    model->nVertices = nVertices;
    model->nTexCoords = nTexCoords;
    model->nNormals = nNormals;
    model->nFaces = nFaces;
    model->hasTexCoords = nTexCoords > 0 ? true : false;
    model->hasNormals = nNormals > 0 ? true : false;

    return true;
}




bool mallocModel(obj_model_t *model) {
    if(model->nVertices > 0) {
        model->vertices = calloc(model->nVertices, sizeof(obj_vertex_t));
        if(!(model->vertices)) { return false; }
    }
    if(model->nTexCoords > 0) {
        model->texcoords = calloc(model->nTexCoords, sizeof(obj_texcoord_t));
        if(!(model->texcoords)) { return false; }
    }
    if(model->nNormals > 0) {
        model->normals = calloc(model->nNormals, sizeof(obj_normal_t));
        if(!(model->normals)) { return false; }
    }
    if(model->nFaces > 0) {
        model->faces = calloc(model->nFaces, sizeof(obj_face_t));
        if(!(model->faces)) { return false; }
    }
    return true;
}




bool objFree(obj_model_t *model) {
    if(!model) {
        return false;
    }

    if(model->vertices) {
        free(model->vertices);
        model->vertices = NULL;
    }
    if(model->texcoords) {
        free(model->texcoords);
        model->texcoords = NULL;
    }
    if(model->normals) {
        free(model->normals);
        model->normals = NULL;
    }
    if(model->faces) {
        for(int i=0; i<model->nFaces; i++) {
            obj_face_t face = model->faces[i];
            if(face.vert_indices) {
                free(face.vert_indices);
            }
            if(face.tex_indices) {
                free(face.tex_indices);
            }
            if(face.norm_Indices) {
                free(face.norm_Indices);
            }
        }
        free(model->faces);
        model->faces = NULL;
    }

}




bool secondPass(FILE *fp, obj_model_t *model) {

    obj_vertex_t *pvert = model->vertices;
    obj_texcoord_t *ptex = model->texcoords;
    obj_normal_t *pnorm = model->normals;
    obj_face_t *pface = model->faces;

    char buf[256];
    while(fgets(buf, sizeof(buf), fp) != NULL) {

        if(buf[0] == 'v') {
            if (buf[1] == ' ') {
                // VERTEX
                sscanf(buf+2, "%f %f %f", &pvert->x, &pvert->y, &pvert->z);
                pvert++;

            } else if (buf[1] == 't') {
                // TEXCOORD
                sscanf(buf+2, "%f %f %f", &ptex->u, &ptex->v, &ptex->w);
                ptex++;

            } else if (buf[1] == 'n') {
                // NORMAL
                sscanf(buf+2, "%f %f %f", &pnorm->x, &pnorm->y, &pnorm->z);
                pnorm++;
            }
        } else if(buf[0] == 'f') {
            // FACE
            pface->vert_indices = calloc(3, sizeof(int));
            pface->tex_indices = calloc(3, sizeof(int));
            pface->norm_Indices = calloc(3, sizeof(int));

            int v00, v01, v02;
            int v10, v11, v12;
            int v20, v21, v22;

            sscanf(buf+2, "%d/%d/%d %d/%d/%d %d/%d/%d", &v00, &v10, &v20, &v01, &v11, &v21, &v02, &v12, &v22);

            pface->vert_indices[0] = v00;
            pface->vert_indices[1] = v01;
            pface->vert_indices[2] = v02;
            pface->tex_indices[0] = v10;
            pface->tex_indices[1] = v11;
            pface->tex_indices[2] = v12;
            pface->norm_Indices[0] = v20;
            pface->norm_Indices[1] = v21;
            pface->norm_Indices[2] = v22;
            pface++;

        } else {
        }

    }

    return true;
}




bool objParse(char *filename, obj_model_t *modelOut) {

    // open file
    FILE *fp;
    fp = fopen(filename, "r");
    if(fp == NULL) {
        printf("Could not open file %s", filename);
        return false;
    }

    obj_model_t model;

    // first pass
    if(!firstPass(fp, &model)) {
        fclose(fp);
        return false;
    }
    rewind(fp);

    // allocate memory for model
    if(!mallocModel(&model)) {
        fclose(fp);
        return false;
    }

    // second pass
    if(!secondPass(fp, &model)) {
        fclose(fp);
        return false;
    }

    *modelOut = model;

    fclose(fp);
    return true;
}