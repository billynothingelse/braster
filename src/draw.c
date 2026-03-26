#include "draw.h"

void draw_line(frame_image image, i32 x0, i32 y0, i32 x1, i32 y1, color_t color)
{
    u8 steep = 0;

    if (abs(x0 - x1) < abs(y0 - y1)) {
        swap(x0, y0);
        swap(x1, y1);
        steep = 1;
    }

    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    i32 dx = x1 - x0;
    i32 dy = y1 - y0;
    i32 derror2 = abs(dy) * 2;
    i32 error2 = 0;
    i32 y = y0;

    for (i32 x = x0; x <= x1; x++) {
        if (steep == 1) {
            image_set_pixel(&image, y, x, color);
        } else {
            image_set_pixel(&image, x, y, color);
        }
        error2 += derror2;
        if (error2 > dx) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

void draw_triangle_barycentric(frame_image image, vec3_t v0, vec3_t v1, vec3_t v2, vec4_t uv0, vec4_t uv1, vec4_t uv2, tga_image_t* normal_map, tga_image_t* diffuse, float *zbuffer)
{
    i32 xmin = MIN(MIN(v0.x, v1.x), v2.x);
    i32 ymin = MIN(MIN(v0.y, v1.y), v2.y);
    i32 xmax = MAX(MAX(v0.x, v1.x), v2.x);
    i32 ymax = MAX(MAX(v0.y, v1.y), v2.y);

    vec3_t light_dir = { 0, 0, 1 };
    vec2_t p;
    for (p.y = ymin; p.y <= ymax; p.y++)
    {
        for (p.x = xmin; p.x <= xmax; p.x++)
        {
            vec3_t bc = barycentric(v0, v1, v2, p);

            if (bc.x < 0 || bc.y < 0 || bc.z < 0)
            {
                continue;
            }

            if (p.x >= 0 && p.y >= 0 && p.x < image.width && p.y < image.height) {
                float pz = v0.z * bc.x + v1.z * bc.y + v2.z * bc.z;
                if (zbuffer[(i32)(p.x + p.y * image.width)] < pz)
                {
                    zbuffer[(i32)(p.x + p.y * image.width)] = pz;

                    float u = uv0.x * bc.x + uv1.x * bc.y + uv2.x * bc.z;
                    float v = uv0.y * bc.x + uv1.y * bc.y + uv2.y * bc.z;

                    i32 tx = (i32)(u * ((*normal_map).header.width - 1));
                    i32 ty = (i32)(v * ((*normal_map).header.height - 1));
                    tx = MAX(0, MIN(tx, (*normal_map).header.width - 1));
                    ty = MAX(0, MIN(ty, (*normal_map).header.height - 1));
                    i32 bpp = (*normal_map).header.pixel_depth / 8;
                    i32 idx = (ty * (*normal_map).header.width + tx) * bpp;

                    // encode from 0-255 to -1 to 1
                    vec3_t n;
                    n.x = ((*normal_map).data[idx + 2] / 255.0f) * 2.0f - 1.0f;
                    n.y = ((*normal_map).data[idx + 1] / 255.0f) * 2.0f - 1.0f;
                    n.z = ((*normal_map).data[idx + 0] / 255.0f) * 2.0f - 1.0f;

                    // reflected light
                    // R = 2 * (N dot L) * N - L
                    vec3_t reflected_light;
                    reflected_light.x = n.x * 2.0f * (n.x * light_dir.x + n.y * light_dir.y + n.z * light_dir.z) - light_dir.x;
                    reflected_light.y = n.y * 2.0f * (n.x * light_dir.x + n.y * light_dir.y + n.z * light_dir.z) - light_dir.y;
                    reflected_light.z = n.z * 2.0f * (n.x * light_dir.x + n.y * light_dir.y + n.z * light_dir.z) - light_dir.z;

                    // specular intensity
                    float specular = powf(MAX(0.0f, reflected_light.z), 32.0f);

                    float dot = n.x * light_dir.x + n.y * light_dir.y + n.z * light_dir.z;
                    if (dot > 0.0f) {
                        i32 diffuse_bpp = (*diffuse).header.pixel_depth / 8;
                        i32 diffuse_x = (i32)(u * ((*diffuse).header.width - 1));
                        i32 diffuse_y = (i32)(v * ((*diffuse).header.height - 1));
                        i32 diffuse_idx = (diffuse_y * (*diffuse).header.width + diffuse_x) * diffuse_bpp;

                        u8 tex_b = (*diffuse).data[diffuse_idx + 0];
                        u8 tex_g = (*diffuse).data[diffuse_idx + 1];
                        u8 tex_r = (*diffuse).data[diffuse_idx + 2];

                        color_t col = { 
                            (u8)(tex_r * dot + 255.0f * specular), 
                            (u8)(tex_g * dot + 255.0f * specular), 
                            (u8)(tex_b * dot + 255.0f * specular) 
                        };
                        image_set_pixel(&image, (i32)p.x, (i32)p.y, col);
                    }
                }
            }
        }
    }
}
