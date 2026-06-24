#include <uc8151.hpp>
#include <pico_graphics.hpp>
#include <button.hpp>

#include "ImageData.h"


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

UC8151 uc8151(296, 128, ROTATE_0);
PicoGraphics_Pen1BitY graphics(uc8151.width, uc8151.height, nullptr);

Button button_a(Pin::A);
Button button_c(Pin::C);
Button button_d(Pin::D);
Button button_e(Pin::E);


int main() {

    stdio_init_all();

    graphics.set_pen(0);
    graphics.clear();

    graphics.set_pen(15);
    graphics.set_font("bitmap8");
    graphics.text("Hello World", {0, 0}, 296);
    graphics.text("A / C", {0, 16}, 296);
    sleep_ms(500);

    uc8151.update(&graphics);

    int before_mode = 0;
    int after_mode = 0;

    while(1){

        if (button_a.read()) {
            after_mode = ((after_mode - 1) < 0) ? 4 : (after_mode - 1);
        }
        if (button_c.read()) {
            after_mode = (4 < (after_mode + 1)) ? 0 : (after_mode + 1);
        }

        if (before_mode == after_mode) continue;
        before_mode = after_mode;

        if (after_mode == 0){
            graphics.set_framebuffer((void *)ironImage);
        }

        if (after_mode == 1){
            graphics.set_framebuffer((void *)lennaImage);
        }

        if (after_mode == 2){
            graphics.set_framebuffer((void *)iloveuImage);
        }

        if (after_mode == 3){
            graphics.set_framebuffer((void *)iloveu2Image);
        }

        if (after_mode == 4){
            graphics.set_framebuffer((void *)capImage);
        }

        uc8151.update(&graphics);
        sleep_ms(200);

    }


    return 0;
}
