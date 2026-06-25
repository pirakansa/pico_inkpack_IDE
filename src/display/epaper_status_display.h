#ifndef EPAPER_STATUS_DISPLAY_H
#define EPAPER_STATUS_DISPLAY_H

#include <stddef.h>
#include <stdint.h>

#include <pico_graphics.hpp>
#include <uc8151.hpp>

class EpaperStatusDisplay {
public:
    static constexpr uint16_t WIDTH = 296;
    static constexpr uint16_t HEIGHT = 128;
    static constexpr size_t FRAMEBUFFER_SIZE = WIDTH * HEIGHT / 8;

    EpaperStatusDisplay(
        pimoroni::UC8151 &display,
        pimoroni::PicoGraphics_Pen1BitY &graphics,
        uint8_t *status_framebuffer);

    void render_status(const char *message);
    void render_network_status(
        const char *lan1_ipv4,
        const char *lan1_ipv6,
        const char *lan2_ipv4,
        const char *lan2_ipv6);
    void render_image(const unsigned char *image);

private:
    pimoroni::UC8151 &display;
    pimoroni::PicoGraphics_Pen1BitY &graphics;
    uint8_t *status_framebuffer;
};

#endif
