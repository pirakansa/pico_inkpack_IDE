#include "epaper_preview.h"

#include <stdio.h>

size_t epaper_preview_width_bytes(size_t width) {
    return (width / 8u) + ((width % 8u) ? 1u : 0u);
}

size_t epaper_preview_image_size(size_t width, size_t height) {
    return epaper_preview_width_bytes(width) * height;
}

static int epaper_preview_is_black_pixel(const unsigned char *image, size_t width, size_t x, size_t y) {
    size_t width_bytes = epaper_preview_width_bytes(width);
    unsigned char byte = image[(y * width_bytes) + (x / 8u)];
    unsigned char mask = (unsigned char)(0x80u >> (x % 8u));

    return (byte & mask) == 0u;
}

int epaper_preview_count_colors(
    const unsigned char *image,
    size_t width,
    size_t height,
    size_t *black_pixels,
    size_t *white_pixels
) {
    size_t black_count = 0;
    size_t white_count = 0;

    if (!image || !black_pixels || !white_pixels) {
        return -1;
    }

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            if (epaper_preview_is_black_pixel(image, width, x, y)) {
                ++black_count;
            } else {
                ++white_count;
            }
        }
    }

    *black_pixels = black_count;
    *white_pixels = white_count;
    return 0;
}

int epaper_preview_write_ppm(const char *path, const unsigned char *image, size_t width, size_t height) {
    FILE *file = NULL;

    if (!path || !image) {
        return -1;
    }

    file = fopen(path, "wb");
    if (!file) {
        perror(path);
        return -1;
    }

    if (fprintf(file, "P6\n%zu %zu\n255\n", width, height) < 0) {
        fclose(file);
        return -1;
    }

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            unsigned char pixel[3];
            unsigned char value = epaper_preview_is_black_pixel(image, width, x, y) ? 0x00u : 0xffu;

            pixel[0] = value;
            pixel[1] = value;
            pixel[2] = value;

            if (fwrite(pixel, sizeof(pixel), 1u, file) != 1u) {
                fclose(file);
                return -1;
            }
        }
    }

    fclose(file);
    return 0;
}
