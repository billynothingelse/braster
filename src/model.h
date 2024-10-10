#ifndef __MODEL_H__
#define __MODEL_H__

#include "image.h"
#include "math.h"

typedef union {
    struct faceb {
        vec3i_t a;
        vec3i_t b;
        vec3i_t c;
    };
    int f[8];
} obj_face_t;

typedef union _faceu {
    struct face {
        vec3i_t;
    };
    int u[2];
} face_t;

typedef struct {
    vec3_t* verts;
    int vert_count;
    face_t* faces;
    int face_count;
} model_t;

void model_load(const char* filename, model_t* model);
void model_unload(model_t* model);

void model_render_wireframe(frame_image image, model_t model, color_t color);

#endif // __MODEL_H__