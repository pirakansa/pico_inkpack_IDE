#include "epaper_preview.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

size_t epaper_preview_column_bytes(size_t height) {
    return (height / 8u) + ((height % 8u) ? 1u : 0u);
}

size_t epaper_preview_image_size(size_t width, size_t height) {
    return epaper_preview_column_bytes(height) * width;
}

static int epaper_preview_is_black_pixel(const unsigned char *image, size_t height, size_t x, size_t y) {
    size_t column_bytes = epaper_preview_column_bytes(height);
    unsigned char byte = image[(x * column_bytes) + (y / 8u)];
    unsigned char mask = (unsigned char)(0x80u >> (y % 8u));

    return (byte & mask) == 0u;
}

static void epaper_preview_set_black_pixel(unsigned char *image, size_t height, size_t x, size_t y) {
    size_t column_bytes = epaper_preview_column_bytes(height);
    unsigned char *byte = &image[(x * column_bytes) + (y / 8u)];
    unsigned char mask = (unsigned char)(0x80u >> (y % 8u));

    *byte = (unsigned char)(*byte & (unsigned char)~mask);
}

static const char *epaper_preview_glyph_rows(char c) {
    switch (toupper((unsigned char)c)) {
    case '0': return "111101101101111";
    case '1': return "010110010010111";
    case '2': return "111001111100111";
    case '3': return "111001111001111";
    case '4': return "101101111001001";
    case '5': return "111100111001111";
    case '6': return "111100111101111";
    case '7': return "111001010010010";
    case '8': return "111101111101111";
    case '9': return "111101111001111";
    case 'A': return "010101111101101";
    case 'B': return "110101110101110";
    case 'C': return "111100100100111";
    case 'D': return "110101101101110";
    case 'E': return "111100110100111";
    case 'F': return "111100110100100";
    case 'G': return "111100101101111";
    case 'H': return "101101111101101";
    case 'I': return "111010010010111";
    case 'J': return "001001001101111";
    case 'K': return "101101110101101";
    case 'L': return "100100100100111";
    case 'M': return "101111111101101";
    case 'N': return "101111111111101";
    case 'O': return "111101101101111";
    case 'P': return "111101111100100";
    case 'Q': return "111101101111001";
    case 'R': return "110101110101101";
    case 'S': return "111100111001111";
    case 'T': return "111010010010010";
    case 'U': return "101101101101111";
    case 'V': return "101101101101010";
    case 'W': return "101101111111101";
    case 'X': return "101101010101101";
    case 'Y': return "101101010010010";
    case 'Z': return "111001010100111";
    case '.': return "000000000000010";
    case ':': return "000010000010000";
    case '-': return "000000111000000";
    case '/': return "001001010100100";
    case ' ': return "000000000000000";
    default: return "111001010000010";
    }
}

void epaper_preview_clear(unsigned char *image, size_t width, size_t height) {
    if (!image) {
        return;
    }

    memset(image, 0xff, epaper_preview_image_size(width, height));
}

void epaper_preview_draw_text(
    unsigned char *image,
    size_t width,
    size_t height,
    size_t x,
    size_t y,
    size_t scale,
    const char *text
) {
    size_t cursor_x = x;

    if (!image || !text || scale == 0u) {
        return;
    }

    for (size_t index = 0; text[index] != '\0'; ++index) {
        const char *glyph = epaper_preview_glyph_rows(text[index]);
        for (size_t row = 0; row < 5u; ++row) {
            for (size_t col = 0; col < 3u; ++col) {
                if (glyph[(row * 3u) + col] != '1') {
                    continue;
                }
                for (size_t sy = 0; sy < scale; ++sy) {
                    for (size_t sx = 0; sx < scale; ++sx) {
                        size_t pixel_x = cursor_x + (col * scale) + sx;
                        size_t pixel_y = y + (row * scale) + sy;
                        if (pixel_x < width && pixel_y < height) {
                            epaper_preview_set_black_pixel(image, height, pixel_x, pixel_y);
                        }
                    }
                }
            }
        }
        cursor_x += 4u * scale;
        if (cursor_x >= width) {
            return;
        }
    }
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
            if (epaper_preview_is_black_pixel(image, height, x, y)) {
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
            unsigned char value = epaper_preview_is_black_pixel(image, height, x, y) ? 0x00u : 0xffu;

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
