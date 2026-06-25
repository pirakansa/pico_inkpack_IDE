#ifndef SCREEN_STATE_H
#define SCREEN_STATE_H

#include <stdint.h>

class ScreenState {
public:
    bool is_showing_startup_image() const;
    bool handle_button_mask(uint8_t button_mask);

private:
    bool showing_startup_image = true;
};

#endif
