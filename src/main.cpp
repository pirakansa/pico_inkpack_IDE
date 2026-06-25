#include <uc8151.hpp>
#include <pico_graphics.hpp>
#include <button.hpp>
#include <tusb.h>

#include "ImageData.h"
#include "app/screen_state.h"
#include "display/epaper_status_display.h"
#include "usb_status/usb_status.h"


using namespace pimoroni;

enum Pin {
    A           = 12,
    B           = 13,
    C           = 14,
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
Button button_b(Pin::B);
Button button_c(Pin::C);

uint8_t read_button_mask() {
    uint8_t mask = 0;
    if (button_a.raw()) {
        mask |= USB_STATUS_BUTTON_A_MASK;
    }
    if (button_b.raw()) {
        mask |= USB_STATUS_BUTTON_B_MASK;
    }
    if (button_c.raw()) {
        mask |= USB_STATUS_BUTTON_C_MASK;
    }
    return mask;
}

void render_status_message(const char *status_message) {
    epaper_status_display.render_status(status_message[0] == '\0' ? "(empty)" : status_message);
}

int main() {

    tusb_init();
    stdio_init_all();

    epaper_status_display.render_image(lennaImage);

    char status_message[USB_STATUS_TEXT_SIZE] = "Waiting for hidraw input";
    uint8_t button_report[USB_STATUS_REPORT_SIZE] = {};
    uint8_t last_reported_button_mask = 0xff;
    uint8_t pending_button_mask = 0;
    bool button_report_pending = true;
    ScreenState screen_state;

    while(1){
        tud_task();

        if (usb_status_take_message(status_message, sizeof(status_message))) {
            if (!screen_state.is_showing_startup_image()) {
                render_status_message(status_message);
            }
        }

        uint8_t button_mask = read_button_mask();
        if (screen_state.handle_button_mask(button_mask)) {
            render_status_message(status_message);
        }
        if (button_mask != last_reported_button_mask) {
            pending_button_mask = button_mask;
            button_report_pending = true;
        }
        if (button_report_pending && tud_hid_ready()) {
            size_t report_size = usb_status_build_button_report(
                button_report,
                sizeof(button_report),
                pending_button_mask);
            if (report_size > 0 && tud_hid_report(0, button_report, report_size)) {
                last_reported_button_mask = pending_button_mask;
                button_report_pending = false;
            }
        }

        sleep_ms(10);

    }


    return 0;
}
