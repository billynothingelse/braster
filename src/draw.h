#ifndef __DRAW_H__
#define __DRAW_H__

#include "image.h"
#include "math.h"
#include "tga.h"

void draw_line(frame_image image, int x0, int y0, int x1, int y1, color_t color);

void draw_triangle_barycentric(frame_image image, vec3_t v0, vec3_t v1, vec3_t v2, vec4_t uv0, vec4_t uv1, vec4_t uv2, tga_image_t* normal_map, tga_image_t* diffuse, float *zbuffer);

#endif // __DRAW_H__