#include "epaper_preview.h"
#include "ImageData.h"

#include <stdio.h>

typedef struct {
    const char *name;
    const unsigned char *image;
} preview_image_t;

static int write_preview(const char *output_dir, const preview_image_t *preview) {
    char path[512];
    size_t black_pixels = 0;
    size_t white_pixels = 0;

    if (snprintf(path, sizeof(path), "%s/%s.ppm", output_dir, preview->name) >= (int)sizeof(path)) {
        fprintf(stderr, "preview path is too long\n");
        return -1;
    }

    if (epaper_preview_write_ppm(path, preview->image, EPAPER_PREVIEW_WIDTH, EPAPER_PREVIEW_HEIGHT) != 0) {
        return -1;
    }

    if (epaper_preview_count_colors(
        preview->image,
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
    const preview_image_t previews[] = {
        {"lenna", lennaImage},
        {"iron", ironImage},
        {"iloveu", iloveuImage},
        {"iloveu2", iloveu2Image},
        {"cap", capImage},
    };

    if (argc > 1) {
        output_dir = argv[1];
    }

    for (size_t i = 0; i < sizeof(previews) / sizeof(previews[0]); ++i) {
        if (write_preview(output_dir, &previews[i]) != 0) {
            return 1;
        }
    }

    return 0;
}
