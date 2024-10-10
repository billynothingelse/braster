#include "math.h"
#include <float.h>

vec3_t vec3_cross(vec3_t a, vec3_t b)
{
    vec3_t out;
    out.x = (a.y * b.z) - (a.z * b.y);
    out.y = (a.z * b.x) - (a.x * b.z);
    out.z = (a.x * b.y) - (a.y * b.x);
    return out;
}

vec3_t vec3_sub(vec3_t a, vec3_t b)
{
    vec3_t out;
    out.x = a.x - b.x;
    out.y = a.y - b.y;
    out.z = a.z - b.z;
    return out;
}

float vec3_length(vec3_t v)
{
    float a = v.x * v.x + v.y * v.y + v.z * v.z;
    return sqrtf(a);
}

vec3_t vec3_normalize(vec3_t v)
{
    float length = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
    vec3_t out;
    out.x = v.x / length;
    out.y = v.y / length;
    out.z = v.z / length;
    return out;
}

vec3_t barycentric(vec3_t a, vec3_t b, vec3_t c, vec2_t p)
{
    vec3_t out = { 0, 0, 0 };

    vec3_t ux = { c.x - a.x, b.x - a.x, a.x - p.x };
    vec3_t uy = { c.y - a.y, b.y - a.y, a.y - p.y };

    vec3_t u = vec3_cross(ux, uy);

    if (fabsf(u.z) < 1) {
        out = (vec3_t) { -1, -1, -1 };
        return out;
    }
    out.x = 1.0f - ((u.x + u.y) / u.z);
    out.y = u.y / u.z;
    out.z = u.x / u.z;
    return out;
}

void mat4_set(mat4_t* mat, float* m)
{
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            (*mat).m[i][j] = m[j + 4 * i];
        }
    }
}

mat4_t mat4_identity()
{
    mat4_t out;
    float m[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    mat4_set(&out, m);
    return out;
}

mat4_t mat4_make(float d00, float d01, float d02, float d03,
                 float d10, float d11, float d12, float d13,
                 float d20, float d21, float d22, float d23,
                 float d30, float d31, float d32, float d33)
{
    mat4_t m;
    m.m[0][0] = d00;
    m.m[0][1] = d01;
    m.m[0][2] = d02;
    m.m[0][3] = d03;
    m.m[1][0] = d10;
    m.m[1][1] = d11;
    m.m[1][2] = d12;
    m.m[1][3] = d13;
    m.m[2][0] = d20;
    m.m[2][1] = d21;
    m.m[2][2] = d22;
    m.m[2][3] = d23;
    m.m[3][0] = d30;
    m.m[3][1] = d31;
    m.m[3][2] = d32;
    m.m[3][3] = d33;
    return m;
}

mat4_t mat4_mul(mat4_t a, mat4_t b)
{
    mat4_t r = {0};
    for (int i = 0; i < 4; i++) {
        r.m[i][0] = a.m[i][0] * b.m[0][0] + a.m[i][1] * b.m[1][0] + a.m[i][2] * b.m[2][0] + a.m[i][3] * b.m[3][0];
        r.m[i][1] = a.m[i][0] * b.m[0][1] + a.m[i][1] * b.m[1][1] + a.m[i][2] * b.m[2][1] + a.m[i][3] * b.m[3][1];
        r.m[i][2] = a.m[i][0] * b.m[0][2] + a.m[i][1] * b.m[1][2] + a.m[i][2] * b.m[2][2] + a.m[i][3] * b.m[3][2];
        r.m[i][3] = a.m[i][0] * b.m[0][3] + a.m[i][1] * b.m[1][3] + a.m[i][2] * b.m[2][3] + a.m[i][3] * b.m[3][3];
    }
    return r;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v)
{
    vec4_t result;
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
    return result;
}

mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar)
{
    mat4_t out;

    out.m[0][0] = aspect * (1 / tan(fov / 2));
    out.m[1][1] = 1 / tan(fov / 2);
    out.m[2][2] = zfar / (zfar - znear);
    out.m[3][3] = (-zfar * znear) / (zfar - znear);
    out.m[3][2] = 1.0;

    return out;
}

mat4_t vec3_to_mat4(vec3_t v)
{
    mat4_t m;
    m.m[0][0] = v.x;
    m.m[1][0] = v.y;
    m.m[2][0] = v.z;
    m.m[3][0] = 1.f;
    return m;
}

vec3_t mat4_to_vec3(mat4_t m)
{
    vec3_t v;
    v.x = m.m[0][0] / m.m[3][0];
    v.y = m.m[1][0] / m.m[3][0];
    v.z = m.m[2][0] / m.m[3][0];
    return v;
}