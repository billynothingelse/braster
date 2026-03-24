#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "types.h"
#include "math.h"

typedef struct
{
    i32 width;
    i32 height;
    u8* data;
} frame_image;

void image_init(frame_image* image, i32 width, i32 height);
void image_set_pixel(frame_image* image, i32 x, i32 y, color_t color);
void image_render(frame_image image, const char* filename);

#endif // __IMAGE_H__