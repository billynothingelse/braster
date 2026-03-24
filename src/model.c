#include "model.h"
#include "draw.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void model_load(const char* filename, model_t* model)
{
    FILE* model_file = fopen(filename, "r");
    if (model_file == NULL) {
        printf("[model] unable to load model: %s\n", filename);        
        return;
    }

    (*model).vert_count = 0;
    (*model).face_count = 0;
    (*model).norm_count = 0;
    (*model).uv_count = 0;

    char line[MAX_STRING_LENGTH];
    while (fgets(line, MAX_STRING_LENGTH, model_file) != NULL) {
        if (line[0] == 'v' && line[1] == ' ') {
            printf("[model] found vert %s\n", line);
            (*model).vert_count++;
        }

        if (line[0] == 'f' && line[1] == ' ') {
            printf("[model] found face %s\n", line);
            (*model).face_count++;
        }

        if (line[0] == 'v' && line[1] == 'n') {
            printf("[model] found normal %s\n", line);
            (*model).norm_count++;
        }

        if (line[0] == 'v' && line[1] == 't') {
            printf("[model] found uv %s\n", line);
            (*model).uv_count++;
        }
    }

    (*model).verts = (vec3_t*)malloc(sizeof(vec3_t) * (*model).vert_count);
    (*model).faces = (face_t*)malloc(sizeof(face_t) * (*model).face_count);
    (*model).norms = (vec4_t*)malloc(sizeof(vec4_t) * (*model).norm_count);
    (*model).uvs = (vec4_t*)malloc(sizeof(vec4_t) * (*model).uv_count);

    rewind(model_file);

    i32 vert_index = 0;
    i32 norm_index = 0;
    i32 face_index = 0;
    i32 uv_index = 0;

    while (fgets(line, MAX_STRING_LENGTH, model_file) != NULL) {
        if (line[0] == 'v' && line[1] == ' ') {
            vec3_t v;
            sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z);
            (*model).verts[vert_index] = v;
            vert_index++;

            if (vert_index >= (*model).vert_count) {
                vert_index = 0;
            }
        }

        if (line[0] == 'v' && line[1] == 'n') {
            vec4_t n;
            sscanf(line, "vn %f %f %f", &n.x, &n.y, &n.z);
            n.w = 1.0f; 
            (*model).norms[norm_index] = n;
            norm_index++;

            if (norm_index >= (*model).norm_count) {
                norm_index = 0;
            }
        }

        if (line[0] == 'v' && line[1] == 't') {
            vec4_t uv;
            sscanf(line, "vt %f %f", &uv.x, &uv.y);
            uv.z = 0.0f;
            uv.w = 1.0f;
            (*model).uvs[uv_index] = uv;
            uv_index++;

            if (uv_index >= (*model).uv_count) {
                uv_index = 0;
            }
        }

        if (line[0] == 'f' && line[1] == ' ') {
            obj_face_t face;
            memset(&face, 0, sizeof(obj_face_t));
            i32 d = sscanf(line, "f %i/%i/%i %i/%i/%i %i/%i/%i", 
                &face.vertex.x, &face.uv.x, &face.normal.x,
                &face.vertex.y, &face.uv.y, &face.normal.y,
                &face.vertex.z, &face.uv.z, &face.normal.z);
            if (d == 9) {
                printf("f %i %i/%i/%i %i/%i/%i %i/%i/%i \n", face_index,
                face.vertex.x, face.vertex.y, face.vertex.z,
                face.uv.x, face.uv.y, face.uv.z,
                face.normal.x, face.normal.y, face.normal.z);

                face_t f;
                memset(&f, 0, sizeof(face_t));

                f.v.x = face.vertex.x - 1;
                f.v.y = face.vertex.y - 1;
                f.v.z = face.vertex.z - 1;

                f.n.x = face.normal.x - 1;
                f.n.y = face.normal.y - 1;
                f.n.z = face.normal.z - 1;

                f.t.x = face.uv.x - 1;
                f.t.y = face.uv.y - 1;
                f.t.z = face.uv.z - 1;

                (*model).faces[face_index] = f;
                face_index++;
            }
        }
    }
}

void model_unload(model_t *model)
{
    if (model->faces != NULL && model->verts != NULL && model->norms != NULL && model->uvs != NULL)
    {
        free(model->faces);
        free(model->verts);
        free(model->norms);
        free(model->uvs);
    }
}

void model_render_wireframe(frame_image image, model_t model, color_t color)
{
    if (model.face_count <= 0 || model.vert_count <= 0) {
        return;
    }

    for (i32 i = 0; i < model.face_count; i++) {
        face_t face = model.faces[i];
        for (i32 j = 0; j < 3; j++) {
            vec3_t v0 = model.verts[face.u[j]];
            vec3_t v1 = model.verts[face.u[(j + 1) % 3]];

            i32 x0 = (v0.x + 1.f) * image.width / 2.f;
            i32 y0 = (v0.y + 1.f) * image.height / 2.f;

            i32 x1 = (v1.x + 1.f) * image.width / 2.f;
            i32 y1 = (v1.y + 1.f) * image.height / 2.f;

            draw_line(image, x0, y0, x1, y1, color);
        }
    }
}
