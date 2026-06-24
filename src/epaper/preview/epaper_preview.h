#ifndef _EPAPER_PREVIEW_H_
#define _EPAPER_PREVIEW_H_

#include <stddef.h>

#define EPAPER_PREVIEW_WIDTH 296u
#define EPAPER_PREVIEW_HEIGHT 128u

size_t epaper_preview_column_bytes(size_t height);
size_t epaper_preview_image_size(size_t width, size_t height);
int epaper_preview_write_ppm(const char *path, const unsigned char *image, size_t width, size_t height);
int epaper_preview_count_colors(
    const unsigned char *image,
    size_t width,
    size_t height,
    size_t *black_pixels,
    size_t *white_pixels
);

#endif
