#ifndef DISPLAY_STATE_H
#define DISPLAY_STATE_H

class DisplayState {
public:
    static constexpr int NO_IMAGE_MODE = -1;

    void show_status();
    void select_previous_image(int image_count);
    void select_next_image(int image_count);
    bool take_pending_image_mode(int *mode);

    int image_mode() const;

private:
    int selected_image_mode = NO_IMAGE_MODE;
    int rendered_image_mode = NO_IMAGE_MODE;
};

#endif
