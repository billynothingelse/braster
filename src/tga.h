#ifndef __TGA_H__
#define __TGA_H__

#include "math.h"
#include "types.h"

#pragma pack(push, 1)
typedef struct {
    u8 id_length;
    u8 color_map_type;
    u8 image_type;
    u16 color_map_first_entry_index;
    u16 color_map_length;
    u8 color_map_entry_size;
    u16 x_origin;
    u16 y_origin;
    u16 width;
    u16 height;
    u8 pixel_depth;
    u8 image_descriptor;
} tga_header_t;
#pragma pack(pop)

typedef struct {
    tga_header_t header;
    u8* data;
} tga_image_t;

typedef struct {
    u8 bgra[4];
    u8 bytes_per_pixel;
} tga_color_t;

void tga_load(const char* filename, tga_image_t* image);
void tga_unload(tga_image_t* image);

color_t tga_get_pixel(tga_image_t* image, vec2_t uv);
float tga_get_pixel_intensity(tga_image_t* image, vec2_t uv);

#endif // __TGA_H__