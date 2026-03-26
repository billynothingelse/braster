#include "tga.h"
#include <stdio.h>
#include <stdlib.h>

static i32 tga_bytes_per_pixel(const tga_image_t* image)
{
    return image->header.pixel_depth / 8;
}

static i32 tga_read_pixel(FILE* file, i32 bpp, u8* out)
{
    return fread(out, 1, (size_t)bpp, file) == (size_t)bpp;
}

void tga_load(const char* filename, tga_image_t* image)
{
    image->data = NULL;

    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("[tga] unable to load image: %s\n", filename);
        return;
    }

    fread(&image->header, sizeof(tga_header_t), 1, file);

    if (image->header.width <= 0 || image->header.height <= 0) {
        printf("[tga] invalid image dimensions: %d x %d\n", image->header.width, image->header.height);
        fclose(file);
        return;
    }

    {
        i32 bpp = tga_bytes_per_pixel(image);
        u64 image_size = (u64)image->header.width * (u64)image->header.height * (u64)bpp;

        if (bpp < 1 || bpp > 8) {
            printf("[tga] unsupported pixel depth: %d\n", image->header.pixel_depth);
            fclose(file);
            return;
        }

        image->data = (u8*)malloc((size_t)image_size);
        if (image->data == NULL) {
            printf("[tga] unable to allocate image buffer (%llu bytes)\n", image_size);
            fclose(file);
            return;
        }

        if (image->header.id_length > 0) {
            fseek(file, image->header.id_length, SEEK_CUR);
        }
    }

    if (image->header.image_type == 2 || image->header.image_type == 3) {
        i32 bpp = tga_bytes_per_pixel(image);
        u64 image_size = (u64)image->header.width * (u64)image->header.height * (u64)bpp;
        if (fread(image->data, 1, (size_t)image_size, file) != (size_t)image_size) {
            printf("[tga] unable to read raw image data\n");
            free(image->data);
            image->data = NULL;
        }
    } else if (image->header.image_type == 10 || image->header.image_type == 11) {
        u64 pixel_count = image->header.width * image->header.height;
        u64 current_pixel = 0;
        u64 current_byte = 0;
        i32 bpp = tga_bytes_per_pixel(image);
        u8 pixel[4] = { 0 };

        do {
            u8 chunk_header = 0;
            fread(&chunk_header, sizeof(u8), 1, file);

            if (chunk_header < 128) {
                chunk_header++;

                for (i32 i = 0; i < chunk_header; i++) {
                    if (!tga_read_pixel(file, bpp, pixel)) {
                        printf("[tga] unable to read raw chunk pixel\n");
                        free(image->data);
                        image->data = NULL;
                        fclose(file);
                        return;
                    }
                    for (i32 j = 0; j < bpp; j++) {
                        image->data[current_byte + j] = pixel[j];
                    }
                    current_byte += bpp;
                    current_pixel++;
                }
            } else {
                chunk_header -= 127;

                if (!tga_read_pixel(file, bpp, pixel)) {
                    printf("[tga] unable to read rle chunk pixel\n");
                    free(image->data);
                    image->data = NULL;
                    fclose(file);
                    return;
                }

                for (i32 i = 0; i < chunk_header; i++) {
                    for (i32 j = 0; j < bpp; j++) {
                        image->data[current_byte + j] = pixel[j];
                    }
                    current_byte += bpp;
                    current_pixel++;
                }
            }

        } while (current_pixel < pixel_count);
    } else {
        printf("[tga] unsupported image type: %d\n", image->header.image_type);
        free(image->data);
        image->data = NULL;
    }

    fclose(file);
}

void tga_unload(tga_image_t* image)
{
    if (image && image->data != NULL) {
        free(image->data);
        image->data = NULL;
    }
}

color_t tga_get_pixel(tga_image_t* image, vec2_t uv)
{
    i32 bpp = (*image).header.pixel_depth / 8;
    i32 tx = (i32)(uv.x * ((*image).header.width - 1));
    i32 ty = (i32)(uv.y * ((*image).header.height - 1));

    // clamp to bounds
    if (tx < 0) tx = 0;
    if (ty < 0) ty = 0;
    if (tx >= (*image).header.width) tx = (*image).header.width - 1;
    if (ty >= (*image).header.height) ty = (*image).header.height - 1;

    i32 idx = (ty * (*image).header.width + tx) * bpp;

    color_t col = {
        (*image).data[idx + 2],
        (*image).data[idx + 1],
        (*image).data[idx + 0]
    };
    return col;
}

float tga_get_pixel_intensity(tga_image_t* image, vec2_t uv)
{
    i32 tx = (i32)(uv.x * ((*image).header.width - 1));
    i32 ty = (i32)(uv.y * ((*image).header.height - 1));

    if (tx < 0) tx = 0;
    if (ty < 0) ty = 0;
    if (tx >= (*image).header.width) tx = (*image).header.width - 1;
    if (ty >= (*image).header.height) ty = (*image).header.height - 1;

    i32 idx = ty * (*image).header.width + tx;

    return (*image).data[idx] / 255.0f;
}
