#include "epaper_preview.h"
#include "ImageData.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    const char *name;
    const unsigned char *image;
} preview_image_t;

static const size_t STATUS_SCREEN_MARGIN_X = 8;
static const size_t STATUS_SCREEN_TITLE_Y = 8;
static const size_t ADDRESS_SCREEN_FIRST_LINE_Y = 32;
static const size_t ADDRESS_SCREEN_LINE_SPACING = 22;
static const size_t ADDRESS_SCREEN_TEXT_SCALE = 1;

static void draw_address_line(unsigned char *image, size_t slot, const char *text, size_t y) {
    char line[64];

    snprintf(line, sizeof(line), "%zu:%s", slot, text);
    epaper_preview_draw_text(
        image,
        EPAPER_PREVIEW_WIDTH,
        EPAPER_PREVIEW_HEIGHT,
        STATUS_SCREEN_MARGIN_X,
        y,
        ADDRESS_SCREEN_TEXT_SCALE,
        line);
}

static void draw_address_preview(unsigned char *image) {
    epaper_preview_clear(image, EPAPER_PREVIEW_WIDTH, EPAPER_PREVIEW_HEIGHT);
    epaper_preview_draw_text(
        image,
        EPAPER_PREVIEW_WIDTH,
        EPAPER_PREVIEW_HEIGHT,
        STATUS_SCREEN_MARGIN_X,
        STATUS_SCREEN_TITLE_Y,
        2,
        "IP addresses");
    draw_address_line(image, 1, "192.0.2.101", ADDRESS_SCREEN_FIRST_LINE_Y);
    draw_address_line(image, 2, "2001:db8::101", ADDRESS_SCREEN_FIRST_LINE_Y + ADDRESS_SCREEN_LINE_SPACING);
    draw_address_line(image, 3, "192.0.2.202", ADDRESS_SCREEN_FIRST_LINE_Y + (ADDRESS_SCREEN_LINE_SPACING * 2));
    draw_address_line(image, 4, "2001:db8::202", ADDRESS_SCREEN_FIRST_LINE_Y + (ADDRESS_SCREEN_LINE_SPACING * 3));
}

static void draw_uptime_preview(unsigned char *image) {
    epaper_preview_clear(image, EPAPER_PREVIEW_WIDTH, EPAPER_PREVIEW_HEIGHT);
    epaper_preview_draw_text(
        image,
        EPAPER_PREVIEW_WIDTH,
        EPAPER_PREVIEW_HEIGHT,
        STATUS_SCREEN_MARGIN_X,
        STATUS_SCREEN_TITLE_Y,
        2,
        "USB uptime");
    epaper_preview_draw_text(
        image,
        EPAPER_PREVIEW_WIDTH,
        EPAPER_PREVIEW_HEIGHT,
        STATUS_SCREEN_MARGIN_X,
        54,
        2,
        "0 d 01:23:45");
}

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

static int write_screen_preview(const char *output_dir, const char *name, void (*draw)(unsigned char *)) {
    char path[512];
    size_t black_pixels = 0;
    size_t white_pixels = 0;
    unsigned char *image = (unsigned char *)malloc(epaper_preview_image_size(EPAPER_PREVIEW_WIDTH, EPAPER_PREVIEW_HEIGHT));

    if (!image) {
        fprintf(stderr, "failed to allocate preview framebuffer\n");
        return -1;
    }

    draw(image);
    if (snprintf(path, sizeof(path), "%s/%s.ppm", output_dir, name) >= (int)sizeof(path)) {
        fprintf(stderr, "preview path is too long\n");
        free(image);
        return -1;
    }

    if (epaper_preview_write_ppm(path, image, EPAPER_PREVIEW_WIDTH, EPAPER_PREVIEW_HEIGHT) != 0 ||
        epaper_preview_count_colors(
            image,
            EPAPER_PREVIEW_WIDTH,
            EPAPER_PREVIEW_HEIGHT,
            &black_pixels,
            &white_pixels
        ) != 0) {
        free(image);
        return -1;
    }

    free(image);
    if (black_pixels == 0u || white_pixels == 0u) {
        fprintf(stderr, "screen preview must contain both black and white pixels\n");
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

    if (write_screen_preview(output_dir, "address-screen", draw_address_preview) != 0) {
        return 1;
    }
    if (write_screen_preview(output_dir, "uptime-screen", draw_uptime_preview) != 0) {
        return 1;
    }

    return 0;
}
