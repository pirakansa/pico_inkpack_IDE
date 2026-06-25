#include "app/screen_state.h"

bool ScreenState::is_showing_startup_image() const {
    return showing_startup_image;
}

bool ScreenState::handle_button_mask(uint8_t button_mask) {
    if (!showing_startup_image || button_mask == 0) {
        return false;
    }

    showing_startup_image = false;
    return true;
}
