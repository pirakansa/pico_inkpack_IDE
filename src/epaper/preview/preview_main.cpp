#include "epaper_preview.h"
#include "ImageData.h"

#include <stdio.h>

static int write_lenna_preview(const char *output_dir) {
    char path[512];
    size_t black_pixels = 0;
    size_t white_pixels = 0;

    if (snprintf(path, sizeof(path), "%s/lenna.ppm", output_dir) >= (int)sizeof(path)) {
        fprintf(stderr, "preview path is too long\n");
        return -1;
    }

    if (epaper_preview_write_ppm(path, lennaImage, EPAPER_PREVIEW_WIDTH, EPAPER_PREVIEW_HEIGHT) != 0) {
        return -1;
    }

    if (epaper_preview_count_colors(
        lennaImage,
        EPAPER_PREVIEW_WIDTH,
        EPAPER_PREVIEW_HEIGHT,
        &black_pixels,
        &white_pixels
    ) != 0) {
        return -1;
    }

    if (black_pixels == 0u || white_pixels == 0u) {
        fprintf(stderr, "preview image must contain both black and white pixels\n");
        return -1;
    }

    printf("%s\n", path);
    return 0;
}

int main(int argc, char **argv) {
    const char *output_dir = "build/ui-preview";

    if (argc > 1) {
        output_dir = argv[1];
    }

    if (write_lenna_preview(output_dir) != 0) {
        return 1;
    }

    return 0;
}
