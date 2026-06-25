#include "display/epaper_status_display.h"

#include <stdio.h>

namespace {
constexpr int STATUS_SCREEN_MARGIN_X = 8;
constexpr int STATUS_SCREEN_TITLE_Y = 8;
constexpr int ADDRESS_SCREEN_FIRST_LINE_Y = 32;
constexpr int ADDRESS_SCREEN_LINE_SPACING = 22;
constexpr int ADDRESS_SCREEN_TEXT_SCALE = 1;
constexpr size_t ADDRESS_SCREEN_LINE_SIZE = 64;

void draw_address_line(
    pimoroni::PicoGraphics_Pen1BitY &graphics,
    int slot,
    const char *text,
    int y) {
    char line[ADDRESS_SCREEN_LINE_SIZE] = {};

    snprintf(line, sizeof(line), "%d:%s", slot, text);
    graphics.text(
        line,
        {STATUS_SCREEN_MARGIN_X, y},
        EpaperStatusDisplay::WIDTH - (STATUS_SCREEN_MARGIN_X * 2),
        ADDRESS_SCREEN_TEXT_SCALE);
}
}

EpaperStatusDisplay::EpaperStatusDisplay(
    pimoroni::UC8151 &display,
    pimoroni::PicoGraphics_Pen1BitY &graphics,
    uint8_t *status_framebuffer)
    : display(display),
      graphics(graphics),
      status_framebuffer(status_framebuffer) {
}

void EpaperStatusDisplay::render_status(const char *message) {
    graphics.set_framebuffer(status_framebuffer);
    graphics.set_pen(15);
    graphics.clear();

    graphics.set_pen(0);
    graphics.set_font("bitmap8");
    graphics.text("USB status", {0, 0}, WIDTH, 2);
    graphics.text(message, {0, 28}, WIDTH, 2);
    display.update(&graphics);
}

void EpaperStatusDisplay::render_network_status(
    const char *slot1,
    const char *slot2,
    const char *slot3,
    const char *slot4) {
    graphics.set_framebuffer(status_framebuffer);
    graphics.set_pen(15);
    graphics.clear();

    graphics.set_pen(0);
    graphics.set_font("bitmap8");
    graphics.text(
        "IP addresses",
        {STATUS_SCREEN_MARGIN_X, STATUS_SCREEN_TITLE_Y},
        WIDTH - (STATUS_SCREEN_MARGIN_X * 2),
        2);
    draw_address_line(graphics, 1, slot1, ADDRESS_SCREEN_FIRST_LINE_Y);
    draw_address_line(graphics, 2, slot2, ADDRESS_SCREEN_FIRST_LINE_Y + ADDRESS_SCREEN_LINE_SPACING);
    draw_address_line(graphics, 3, slot3, ADDRESS_SCREEN_FIRST_LINE_Y + (ADDRESS_SCREEN_LINE_SPACING * 2));
    draw_address_line(graphics, 4, slot4, ADDRESS_SCREEN_FIRST_LINE_Y + (ADDRESS_SCREEN_LINE_SPACING * 3));
    display.update(&graphics);
}

void EpaperStatusDisplay::render_uptime(const char *uptime) {
    graphics.set_framebuffer(status_framebuffer);
    graphics.set_pen(15);
    graphics.clear();

    graphics.set_pen(0);
    graphics.set_font("bitmap8");
    graphics.text(
        "USB uptime",
        {STATUS_SCREEN_MARGIN_X, STATUS_SCREEN_TITLE_Y},
        WIDTH - (STATUS_SCREEN_MARGIN_X * 2),
        2);
    graphics.text(
        uptime,
        {STATUS_SCREEN_MARGIN_X, 54},
        WIDTH - (STATUS_SCREEN_MARGIN_X * 2),
        2);
    display.update(&graphics);
}

void EpaperStatusDisplay::render_image(const unsigned char *image) {
    graphics.set_framebuffer((void *)image);
    display.update(&graphics);
}
