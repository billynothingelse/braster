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
        vec3i_t t;
    };
    int u[9];
} face_t;

typedef struct {
    vec3_t* verts;
    i32 vert_count;
    face_t* faces;
    i32 face_count;
    vec4_t* norms;
    i32 norm_count;
    vec4_t* uvs;
    i32 uv_count;
} model_t;

void model_load(const char* filename, model_t* model);
void model_unload(model_t* model);

void model_render_wireframe(frame_image image, model_t model, color_t color);

#endif // __MODEL_H__