#include <uc8151.hpp>
#include <pico_graphics.hpp>
#include <button.hpp>
#include <pico/time.h>
#include <tusb.h>

#include <stdio.h>

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

void render_network_status(char addresses[USB_STATUS_IP_SLOT_COUNT][USB_STATUS_TEXT_SIZE]) {
    epaper_status_display.render_network_status(
        addresses[0],
        addresses[1],
        addresses[2],
        addresses[3]);
}

uint32_t elapsed_seconds_since(absolute_time_t started_at) {
    int64_t elapsed_us = absolute_time_diff_us(started_at, get_absolute_time());
    if (elapsed_us < 0) {
        return 0;
    }
    return (uint32_t)(elapsed_us / 1000000);
}

void format_uptime(char *buffer, size_t buffer_size, uint32_t elapsed_seconds) {
    uint32_t days = elapsed_seconds / 86400;
    uint32_t hours = (elapsed_seconds / 3600) % 24;
    uint32_t minutes = (elapsed_seconds / 60) % 60;
    uint32_t seconds = elapsed_seconds % 60;

    snprintf(
        buffer,
        buffer_size,
        "%lu d %02lu:%02lu:%02lu",
        (unsigned long)days,
        (unsigned long)hours,
        (unsigned long)minutes,
        (unsigned long)seconds);
}

void render_uptime_screen(absolute_time_t usb_started_at, uint32_t *rendered_uptime_minute) {
    uint32_t elapsed_seconds = elapsed_seconds_since(usb_started_at);
    char uptime[32] = {};

    format_uptime(uptime, sizeof(uptime), elapsed_seconds);
    epaper_status_display.render_uptime(uptime);
    *rendered_uptime_minute = elapsed_seconds / 60;
}

void render_current_screen(
    const ScreenState &screen_state,
    char addresses[USB_STATUS_IP_SLOT_COUNT][USB_STATUS_TEXT_SIZE],
    absolute_time_t usb_started_at,
    uint32_t *rendered_uptime_minute) {
    if (screen_state.is_showing_network_status()) {
        render_network_status(addresses);
    } else if (screen_state.is_showing_uptime()) {
        render_uptime_screen(usb_started_at, rendered_uptime_minute);
    }
}

int main() {

    tusb_init();
    absolute_time_t usb_started_at = get_absolute_time();
    stdio_init_all();

    epaper_status_display.render_image(lennaImage);

    char network_addresses[USB_STATUS_IP_SLOT_COUNT][USB_STATUS_TEXT_SIZE] = {
        "waiting",
        "-",
        "-",
        "-"
    };
    uint8_t button_report[USB_STATUS_REPORT_SIZE] = {};
    uint8_t last_reported_button_mask = 0xff;
    uint8_t previous_button_mask = 0;
    uint8_t pending_button_mask = 0;
    bool button_report_pending = true;
    uint32_t rendered_uptime_minute = UINT32_MAX;
    ScreenState screen_state;

    while(1){
        tud_task();

        if (usb_status_take_network_status(network_addresses)) {
            if (screen_state.is_showing_network_status()) {
                render_network_status(network_addresses);
            }
        }

        uint8_t button_mask = read_button_mask();
        uint8_t pressed_button_mask = button_mask & (uint8_t)~previous_button_mask;
        previous_button_mask = button_mask;

        if (screen_state.handle_button_press(pressed_button_mask)) {
            render_current_screen(
                screen_state,
                network_addresses,
                usb_started_at,
                &rendered_uptime_minute);
        }
        if (screen_state.is_showing_uptime()) {
            uint32_t uptime_minute = elapsed_seconds_since(usb_started_at) / 60;
            if (uptime_minute != rendered_uptime_minute) {
                render_uptime_screen(usb_started_at, &rendered_uptime_minute);
            }
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
