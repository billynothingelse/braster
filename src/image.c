#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void image_init(frame_image* image, int width, int height)
{
    int pixels = 0;
    (*image).width = width;
    (*image).height = height;
    pixels = (*image).width * (*image).height * 3;

    (*image).data = (u8*)malloc(sizeof(u8) * pixels);
    memset((*image).data, 0, sizeof(u8) * pixels);
}

void image_set_pixel(frame_image* image, int x, int y, color_t color)
{
    if (y <= 0) {
        y = 1;
    }

    if (y >= 800) {
        y = 800 - 1;
    }
    
    int index = (((*image).height - y) * (*image).width + x) * 3;

    memcpy((*image).data + index, (const char*)&color, 3);
}

void image_render(frame_image image, const char* filename)
{
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Failed to write %s output image\n", filename);
        return;
    }

    // initialize PPM header
    fprintf(file, "P6\n%d %d\n%d\n", image.width, image.height, 255);

    // write direct image data
    fwrite(image.data, sizeof(u8), 3 * image.width * image.height, file);
    
    fclose(file);
}