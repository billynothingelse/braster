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

void draw_triangle_barycentric(frame_image image, vec3_t v0, vec3_t v1, vec3_t v2, float *zbuffer, color_t col)
{
    int xmin = MIN(MIN(v0.x, v1.x), v2.x);
    int ymin = MIN(MIN(v0.y, v1.y), v2.y);
    int xmax = MAX(MAX(v0.x, v1.x), v2.x);
    int ymax = MAX(MAX(v0.y, v1.y), v2.y);

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

            if (p.x >= 0 && p.y >= 0) {
                float pz = 0;
                pz += v0.z * p.x + p.y * 3;
                pz += v1.z * p.x + p.y * 3;
                pz += v2.z * p.x + p.y * 3;
                if (zbuffer[(int)(p.x + p.y * image.width)] < pz)
                {
                    zbuffer[(int)(p.x + p.y * image.width)] = pz;
                    image_set_pixel(&image, (int)p.x, (int)p.y, col);
                }
            }
        }
    }
}