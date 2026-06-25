#include "display/epaper_status_display.h"

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
    graphics.set_pen(0);
    graphics.clear();

    graphics.set_pen(15);
    graphics.set_font("bitmap8");
    graphics.text("USB status", {0, 0}, WIDTH, 2);
    graphics.text(message, {0, 28}, WIDTH, 2);
    display.update(&graphics);
}
