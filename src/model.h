#ifndef __MODEL_H__
#define __MODEL_H__

#include "image.h"
#include "math.h"

typedef union {
    struct faceb {
        vec3i_t vertex;
        vec3i_t uv;
        vec3i_t normal;
    };
    int f[8];
} obj_face_t;

typedef union _faceu {
    struct face {
        vec3i_t v;
        vec3i_t n;
    };
    int u[6];
} face_t;

typedef struct {
    vec3_t* verts;
    int vert_count;
    face_t* faces;
    int face_count;
    vec3_t* norms;
    int norm_count;
} model_t;

void model_load(const char* filename, model_t* model);
void model_unload(model_t* model);

void model_render_wireframe(frame_image image, model_t model, color_t color);

#endif // __MODEL_H__