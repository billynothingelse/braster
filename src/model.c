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
    }

    (*model).verts = (vec3_t*)malloc(sizeof(vec3_t) * (*model).vert_count);
    (*model).faces = (face_t*)malloc(sizeof(face_t) * (*model).face_count);

    rewind(model_file);

    int index = 0;
    while (fgets(line, MAX_STRING_LENGTH, model_file) != NULL) {
        if (line[0] == 'v' && line[1] == ' ') {
            vec3_t v;
            sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z);
            (*model).verts[index] = v;
            index++;

            if (index >= (*model).vert_count) {
                index = 0;
            }
        }

        if (line[0] == 'f' && line[1] == ' ') {
            obj_face_t face;
            memset(&face, 0, sizeof(obj_face_t));
            int d = sscanf(line, "f %i/%i/%i %i/%i/%i %i/%i/%i", 
                &face.a.x, &face.a.y, &face.a.z,
                &face.b.x, &face.b.y, &face.b.z,
                &face.c.x, &face.c.y, &face.c.z);
            if (d == 9) {
                printf("f %i %i/%i/%i %i/%i/%i %i/%i/%i \n", index,
                face.a.x, face.a.y, face.a.z,
                face.b.x, face.b.y, face.b.z,
                face.c.x, face.c.y, face.c.z);

                face_t f;
                memset(&f, 0, sizeof(face_t));

                f.x = face.a.x - 1;
                f.y = face.b.x - 1;
                f.z = face.c.x - 1;

                (*model).faces[index] = f;
                index++;
            }
        }
    }
}

void model_unload(model_t *model)
{
    if (model->faces != NULL && model->verts != NULL)
    {
        free(model->faces);
        free(model->verts);
    }
}

void model_render_wireframe(frame_image image, model_t model, color_t color)
{
    if (model.face_count <= 0 || model.vert_count <= 0) {
        return;
    }

    for (int i = 0; i < model.face_count; i++) {
        face_t face = model.faces[i];
        for (int j = 0; j < 3; j++) {
            vec3_t v0 = model.verts[face.u[j]];
            vec3_t v1 = model.verts[face.u[(j + 1) % 3]];

            int x0 = (v0.x + 1.f) * image.width / 2.f;
            int y0 = (v0.y + 1.f) * image.height / 2.f;

            int x1 = (v1.x + 1.f) * image.width / 2.f;
            int y1 = (v1.y + 1.f) * image.height / 2.f;

            draw_line(image, x0, y0, x1, y1, color);
        }
    }
}
