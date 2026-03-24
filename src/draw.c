#include "draw.h"

void draw_line(frame_image image, int x0, int y0, int x1, int y1, color_t color)
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

    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = abs(dy) * 2;
    int error2 = 0;
    int y = y0;

    for (int x = x0; x <= x1; x++) {
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

void draw_triangle_barycentric(frame_image image, vec3_t v0, vec3_t v1, vec3_t v2, vec4_t uv0, vec4_t uv1, vec4_t uv2, tga_image_t* normal_map, float *zbuffer)
{
    int xmin = MIN(MIN(v0.x, v1.x), v2.x);
    int ymin = MIN(MIN(v0.y, v1.y), v2.y);
    int xmax = MAX(MAX(v0.x, v1.x), v2.x);
    int ymax = MAX(MAX(v0.y, v1.y), v2.y);

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
                if (zbuffer[(int)(p.x + p.y * image.width)] < pz)
                {
                    zbuffer[(int)(p.x + p.y * image.width)] = pz;

                    float u = uv0.x * bc.x + uv1.x * bc.y + uv2.x * bc.z;
                    float v = uv0.y * bc.x + uv1.y * bc.y + uv2.y * bc.z;

                    int tx = (int)(u * (normal_map->header.width - 1));
                    int ty = (int)(v * ((*normal_map).header.height - 1));
                    tx = MAX(0, MIN(tx, (*normal_map).header.width - 1));
                    ty = MAX(0, MIN(ty, (*normal_map).header.height - 1));
                    int bpp = (*normal_map).header.pixel_depth / 8;
                    int idx = (ty * (*normal_map).header.width + tx) * bpp;

                    // encode from 0-255 to -1 to 1
                    vec3_t n;
                    n.x = ((*normal_map).data[idx + 2] / 255.0f) * 2.0f - 1.0f;
                    n.y = ((*normal_map).data[idx + 1] / 255.0f) * 2.0f - 1.0f;
                    n.z = ((*normal_map).data[idx + 0] / 255.0f) * 2.0f - 1.0f;

                    float dot = n.x * light_dir.x + n.y * light_dir.y + n.z * light_dir.z;
                    if (dot > 0.0f) {
                        u8 intensity = (u8)(dot * 255);
                        color_t col = { intensity, intensity, intensity };
                        image_set_pixel(&image, (int)p.x, (int)p.y, col);
                    }
                }
            }
        }
    }
}