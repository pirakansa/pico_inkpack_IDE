#include "app/display_state.h"

void DisplayState::show_status() {
    selected_image_mode = NO_IMAGE_MODE;
    rendered_image_mode = NO_IMAGE_MODE;
}

void DisplayState::select_previous_image(int image_count) {
    if (image_count <= 0) {
        return;
    }

    if (selected_image_mode == NO_IMAGE_MODE) {
        selected_image_mode = image_count - 1;
    } else {
        --selected_image_mode;
        if (selected_image_mode < 0) {
            selected_image_mode = image_count - 1;
        }
    }
}

void DisplayState::select_next_image(int image_count) {
    if (image_count <= 0) {
        return;
    }

    if (selected_image_mode == NO_IMAGE_MODE) {
        selected_image_mode = 0;
    } else {
        ++selected_image_mode;
        if (selected_image_mode >= image_count) {
            selected_image_mode = 0;
        }
    }
}

bool DisplayState::take_pending_image_mode(int *mode) {
    if (selected_image_mode == NO_IMAGE_MODE || selected_image_mode == rendered_image_mode) {
        return false;
    }

    rendered_image_mode = selected_image_mode;
    if (mode != nullptr) {
        *mode = selected_image_mode;
    }
    return true;
}

int DisplayState::image_mode() const {
    return selected_image_mode;
}
