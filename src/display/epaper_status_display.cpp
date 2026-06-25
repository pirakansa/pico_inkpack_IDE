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

void EpaperStatusDisplay::render_network_status(
    const char *lan1_ipv4,
    const char *lan1_ipv6,
    const char *lan2_ipv4,
    const char *lan2_ipv6) {
    graphics.set_framebuffer(status_framebuffer);
    graphics.set_pen(0);
    graphics.clear();

    graphics.set_pen(15);
    graphics.set_font("bitmap8");
    graphics.text("IP addresses", {0, 0}, WIDTH, 2);
    graphics.text("LAN1 IPv4", {0, 22}, WIDTH, 1);
    graphics.text(lan1_ipv4, {0, 32}, WIDTH, 1);
    graphics.text("LAN1 IPv6", {0, 48}, WIDTH, 1);
    graphics.text(lan1_ipv6, {0, 58}, WIDTH, 1);
    graphics.text("LAN2 IPv4", {0, 74}, WIDTH, 1);
    graphics.text(lan2_ipv4, {0, 84}, WIDTH, 1);
    graphics.text("LAN2 IPv6", {0, 100}, WIDTH, 1);
    graphics.text(lan2_ipv6, {0, 110}, WIDTH, 1);
    display.update(&graphics);
}

void EpaperStatusDisplay::render_image(const unsigned char *image) {
    graphics.set_framebuffer((void *)image);
    display.update(&graphics);
}
