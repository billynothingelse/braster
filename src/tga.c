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

    // read tga header
    fread(&image->header, sizeof(tga_header_t), 1, file);

    if ((*image).header.width <= 0 || (*image).header.height <= 0) {
        printf("[tga] invalid image dimensions: %d x %d\n", (*image).header.width, (*image).header.height);
        fclose(file);
        return;
    }

    {
        i32 bpp = tga_bytes_per_pixel(image);
        u64 image_size = (u64)(*image).header.width * (u64)(*image).header.height * (u64)bpp;

        if (bpp < 3 || bpp > 4) {
            printf("[tga] unsupported pixel depth: %d\n", (*image).header.pixel_depth);
            fclose(file);
            return;
        }

        image->data = (u8*)malloc((size_t)image_size);
        if (image->data == NULL) {
            printf("[tga] unable to allocate image buffer (%llu bytes)\n", image_size);
            fclose(file);
            return;
        }

        if ((*image).header.id_length > 0) {
            fseek(file, (*image).header.id_length, SEEK_CUR);
        }
    }

    if ((*image).header.image_type == 2) {
        i32 bpp = tga_bytes_per_pixel(image);
        u64 image_size = (u64)(*image).header.width * (u64)(*image).header.height * (u64)bpp;
        if (fread((*image).data, 1, (size_t)image_size, file) != (size_t)image_size) {
            printf("[tga] unable to read raw image data\n");
            free((*image).data);
            (*image).data = NULL;
        }
    } else if ((*image).header.image_type == 10) {
        u64 pixel_count = (*image).header.width * (*image).header.height;
        u64 current_pixel = 0;
        u64 current_byte = 0;
        i32 bpp = tga_bytes_per_pixel(image);
        u8 pixel[4] = { 0 };

        do {
            // parse chunks
            u8 chunk_header = 0;
            fread(&chunk_header, sizeof(u8), 1, file);
            
            if (chunk_header < 128) {
                // raw chunk
                chunk_header++;

                for (i32 i = 0; i < chunk_header; i++) {
                    if (!tga_read_pixel(file, bpp, pixel)) {
                        printf("[tga] unable to read raw chunk pixel\n");
                        free((*image).data);
                        (*image).data = NULL;
                        fclose(file);
                        return;
                    }
                    (*image).data[current_byte] = pixel[0];
                    (*image).data[current_byte + 1] = pixel[1];
                    (*image).data[current_byte + 2] = pixel[2];
                    if (bpp == 4) {
                        (*image).data[current_byte + 3] = pixel[3];
                    }
                    current_byte += bpp;
                    current_pixel++;
                }
            } else {
                // rle chunk
                chunk_header -= 127;

                if (!tga_read_pixel(file, bpp, pixel)) {
                    printf("[tga] unable to read rle chunk pixel\n");
                    free((*image).data);
                    (*image).data = NULL;
                    fclose(file);
                    return;
                }

                for (i32 i = 0; i < chunk_header; i++) {
                    (*image).data[current_byte] = pixel[0];
                    (*image).data[current_byte + 1] = pixel[1];
                    (*image).data[current_byte + 2] = pixel[2];
                    if (bpp == 4) {
                        (*image).data[current_byte + 3] = pixel[3];
                    }
                    current_byte += bpp;
                    current_pixel++;
                }
            }

        } while (current_pixel < pixel_count);
    } else {
        printf("[tga] unsupported image type: %d\n", (*image).header.image_type);
        free((*image).data);
        (*image).data = NULL;
    }

    // we're done
    fclose(file);
}

void tga_unload(tga_image_t* image)
{
    if (image && image->data != NULL) {
        free(image->data);
        image->data = NULL;
    }
}

