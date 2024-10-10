#ifndef __DRAW_H__
#define __DRAW_H__

#include "image.h"
#include "math.h"

void draw_line(frame_image image, int x0, int y0, int x1, int y1, color_t color);

void draw_triangle_barycentric(frame_image image, vec3_t v0, vec3_t v1, vec3_t v2, float *zbuffer, color_t col);

#endif // __DRAW_H__