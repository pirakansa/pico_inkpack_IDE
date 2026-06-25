#include <uc8151.hpp>
#include <pico_graphics.hpp>
#include <tusb.h>

#include "display/epaper_status_display.h"
#include "usb_status/usb_status.h"


using namespace pimoroni;

enum Pin {
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

int main() {

    tusb_init();
    stdio_init_all();

    epaper_status_display.render_status("Waiting for hidraw input");
    sleep_ms(500);

    char status_message[USB_STATUS_TEXT_SIZE] = {};

    while(1){
        tud_task();

        if (usb_status_take_message(status_message, sizeof(status_message))) {
            epaper_status_display.render_status(status_message[0] == '\0' ? "(empty)" : status_message);
        }

        sleep_ms(10);

    }


    return 0;
}
