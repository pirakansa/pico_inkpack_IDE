#ifndef SCREEN_STATE_H
#define SCREEN_STATE_H

#include <stdint.h>

class ScreenState {
public:
    enum class Screen {
        STARTUP,
        NETWORK,
        UPTIME
    };

    bool is_showing_startup_image() const;
    bool is_showing_network_status() const;
    bool is_showing_uptime() const;
    Screen current_screen() const;
    bool handle_button_press(uint8_t pressed_button_mask);

private:
    Screen screen = Screen::STARTUP;
};

#endif
