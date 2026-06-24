#include <uc8151.hpp>
#include <pico_graphics.hpp>
#include <button.hpp>
#include <tusb.h>

#include "ImageData.h"
#include "app/display_state.h"
#include "display/epaper_status_display.h"
#include "usb_status/usb_status.h"


using namespace pimoroni;

enum Pin {
    A           = 12,
    B           = 13,
    C           = 14,
    D           = 15,
    E           = 11,
    UP          = 15, // alias for D
    DOWN        = 11, // alias for E
    USER        = 23,
    CS          = 17,
    CLK         = 18,
    MOSI        = 19,
    DC          = 20,
    RESET       = 21,
    BUSY        = 26,
    VBUS_DETECT = 24,
    LED         = 25,
    BATTERY     = 29,
    ENABLE_3V3  = 10
};

UC8151 uc8151(EpaperStatusDisplay::WIDTH, EpaperStatusDisplay::HEIGHT, ROTATE_0);
PicoGraphics_Pen1BitY graphics(uc8151.width, uc8151.height, nullptr);
uint8_t status_framebuffer[EpaperStatusDisplay::FRAMEBUFFER_SIZE];
EpaperStatusDisplay epaper_status_display(uc8151, graphics, status_framebuffer);

Button button_a(Pin::A);
Button button_c(Pin::C);
Button button_d(Pin::D);
Button button_e(Pin::E);

const unsigned char *const mode_images[] = {
    ironImage,
    lennaImage,
    iloveuImage,
    iloveu2Image,
    capImage
};

constexpr int MODE_IMAGE_COUNT = sizeof(mode_images) / sizeof(mode_images[0]);

int main() {

    tusb_init();
    stdio_init_all();

    epaper_status_display.render_status("Waiting for hidraw input");
    sleep_ms(500);

    DisplayState display_state;
    char status_message[USB_STATUS_TEXT_SIZE] = {};

    while(1){
        tud_task();

        if (usb_status_take_message(status_message, sizeof(status_message))) {
            epaper_status_display.render_status(status_message[0] == '\0' ? "(empty)" : status_message);
            display_state.show_status();
        }

        if (button_a.read()) {
            display_state.select_previous_image(MODE_IMAGE_COUNT);
        }
        if (button_c.read()) {
            display_state.select_next_image(MODE_IMAGE_COUNT);
        }

        int image_mode = 0;
        if (display_state.take_pending_image_mode(&image_mode)) {
            epaper_status_display.render_image(mode_images[image_mode]);
            sleep_ms(200);
        }

        sleep_ms(10);

    }


    return 0;
}
