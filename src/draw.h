#ifndef __DRAW_H__
#define __DRAW_H__

#include "image.h"
#include "math.h"

void draw_line(frame_image image, int x0, int y0, int x1, int y1, color_t color);

void draw_triangle_barycentric(frame_image image, vec3_t v0, vec3_t v1, vec3_t v2, vec3_t n0, vec3_t n1, vec3_t n2, float *zbuffer);

#endif // __DRAW_H__