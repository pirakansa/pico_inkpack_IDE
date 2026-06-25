#include "app/screen_state.h"
#include "usb_status/usb_status.h"

bool ScreenState::is_showing_startup_image() const {
    return screen == Screen::STARTUP;
}

bool ScreenState::is_showing_network_status() const {
    return screen == Screen::NETWORK;
}

bool ScreenState::is_showing_uptime() const {
    return screen == Screen::UPTIME;
}

ScreenState::Screen ScreenState::current_screen() const {
    return screen;
}

bool ScreenState::handle_button_press(uint8_t pressed_button_mask) {
    if (pressed_button_mask == 0) {
        return false;
    }

    if (screen == Screen::STARTUP) {
        screen = Screen::NETWORK;
        return true;
    }

    if ((pressed_button_mask & USB_STATUS_BUTTON_A_MASK) != 0 ||
        (pressed_button_mask & USB_STATUS_BUTTON_C_MASK) != 0) {
        screen = screen == Screen::NETWORK ? Screen::UPTIME : Screen::NETWORK;
        return true;
    }

    return false;
}
