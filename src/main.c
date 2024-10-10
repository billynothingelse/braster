#include "image.h"
#include "draw.h"
#include "math.h"
#include <math.h>
#include <float.h>
#include "model.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

mat4_t get_look_at_mat(vec3_t eye, vec3_t center, vec3_t up)
{
    vec3_t z = vec3_normalize(vec3_sub(eye, center));
    vec3_t x = vec3_normalize(vec3_cross(up, z));
    vec3_t y = vec3_cross(z, x);

    mat4_t inv = mat4_make(x.x, x.y, x.z, 0,
                           y.x, y.y, y.z, 0,
                           z.x, z.y, z.z, 0,
                           0,   0,  0,  1);

    mat4_t tr = mat4_make(1, 0, 0, -center.x,
                          0, 1, 0, -center.x,
                          0, 0, 1, -center.z,
                          0, 0, 0, 1);

    return mat4_mul(inv, tr);
}

int main()
{
    frame_image image;
    int height = 800;
    int width = 800;
    char* filename = "frame.ppm";
    float* zbuffer;
    model_t model;

    zbuffer = malloc(sizeof(float) * (height * width));
    memset(zbuffer, 0, sizeof(*zbuffer));

    // initialize frame image
    image_init(&image, width, height);

    // load test model
    model_load("african_head.obj", &model);

    // setup viewport/projection
    vec3_t camera = { 6.f, 0.f, 4.f };
    vec3_t target = { 0.f, 0.f, 0.f };
    vec3_t blunder = { 0.f, 1.f, 0.f };

    float cam_z = vec3_length(vec3_sub(camera, target));

    mat4_t projection_mat = mat4_make(1,0,0,0,
                                        0,1,0,0,
                                        0,0,1,0,
                                        0,0,-1.f / cam_z, 1);
    mat4_t view_matrix = get_look_at_mat(camera, target, blunder);
    mat4_t viewport;
    {
        float w = width - 1;
        float h = height - 1;
        float d = 255.0f;
        viewport = mat4_make(w / 2.0f,  0,  0, w / 2.0f,
                            0, h / 2.0f,    0, h / 2.0f,
                            0,  0,  d / 2.0f, d / 2.0f,
                            0,  0,  0,  1);
    }

    mat4_t transform_mat = mat4_mul(viewport, mat4_mul(projection_mat, view_matrix));

    // draw filled (light from center)
    for (int i = 0; i < model.face_count; i++) {
        face_t face = model.faces[i];
        vec3_t screen_coords[3];
        vec3_t world_coords[3];
        for (int j = 0; j < 3; j++) {
            vec3_t v = model.verts[face.u[j]];

            vec4_t v4 = { v.x, v.y, v.z, 1.0f };
            vec4_t h = mat4_mul_vec4(transform_mat, v4);
            vec4_t v_h = {
                h.x / h.w,
                h.y / h.w,
                h.z / h.w,
                h.w / h.w
            };

            vec3_t d = { v_h.x, v_h.y, v_h.z };
            screen_coords[j] = d;
            world_coords[j] = v;
        }

        vec3_t d1;
        d1.x = world_coords[1].x - world_coords[0].x;
        d1.y = world_coords[1].y - world_coords[0].y;
        d1.z = world_coords[1].z - world_coords[0].z;

        vec3_t d2;
        d2.x = world_coords[2].x - world_coords[0].x;
        d2.y = world_coords[2].y - world_coords[0].y;
        d2.z = world_coords[2].z - world_coords[0].z;

        vec3_t n = vec3_cross(d1, d2);

        float distance = sqrt(n.x * n.x + n.y * n.y + n.z * n.z);

        n.x = n.x / distance;
        n.y = n.y / distance;
        n.z = n.z / distance;

        float direction = (n.x * 0) + (n.y * 0) + (n.z * 1);
        if (direction > 0) {
            color_t light = { direction * 255, direction * 255, direction * 255 };
            draw_triangle_barycentric(image, screen_coords[0], screen_coords[1], screen_coords[2], zbuffer, light);
        }
    }

    // write and export image data to ppm
    image_render(image, filename);

    printf("image rendered\n");

    // cleanup
    model_unload(&model);
    free(zbuffer);

    return 0;
}