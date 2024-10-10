#ifndef __MATH_H__
#define __MATH_H__

#include <math.h>
#include "types.h"

typedef struct
{
    float x;
    float y;
} vec2_t;

typedef struct
{
    i32 x;
    i32 y;
} vec2i_t;

typedef struct
{
    float x;
    float y;
    float z;
} vec3_t;

typedef struct
{
    i32 x;
    i32 y;
    i32 z;
} vec3i_t;

typedef struct
{
    float x;
    float y;
    float z;
    float w;
} vec4_t;

typedef struct
{
    u8 r;
    u8 g;
    u8 b;
} color_t;

typedef struct
{
    float m[4][4];
} mat4_t;


vec3_t vec3_cross(vec3_t a, vec3_t b);
vec3_t vec3_sub(vec3_t a, vec3_t b);
float vec3_length(vec3_t v);
vec3_t vec3_normalize(vec3_t v);
vec3_t barycentric(vec3_t a, vec3_t b, vec3_t c, vec2_t p);

mat4_t mat4_identity();
mat4_t mat4_make(float d00, float d01, float d02, float d03,
                 float d10, float d11, float d12, float d13,
                 float d20, float d21, float d22, float d23,
                 float d30, float d31, float d32, float d33);
mat4_t mat4_mul(mat4_t a, mat4_t b);
vec4_t mat4_mul_vec4(mat4_t m, vec4_t v);
mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar);

mat4_t vec3_to_mat4(vec3_t v);
vec3_t mat4_to_vec3(mat4_t m);


#endif // __MATH_H__