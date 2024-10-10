#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "types.h"
#include "math.h"

typedef struct
{
    int width;
    int height;
    u8* data;
} frame_image;

void image_init(frame_image* image, int width, int height);
void image_set_pixel(frame_image* image, int x, int y, color_t color);
void image_render(frame_image image, const char* filename);

#endif // __IMAGE_H__