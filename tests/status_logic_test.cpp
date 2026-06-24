#include "app/display_state.h"
#include "usb_status/usb_status.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

static void test_formats_hid_report_text() {
    const uint8_t report[] = {
        'I', 'P', ':', ' ', '1', '9', '2', '\r', '\n', 'O', 'K', 0, 0, 0
    };
    char message[USB_STATUS_TEXT_SIZE] = {};

    size_t written = usb_status_format_message(message, sizeof(message), report, sizeof(report));

    assert(written == strlen("IP: 192\nOK"));
    assert(strcmp(message, "IP: 192\nOK") == 0);
}

static void test_limits_hid_report_to_report_size() {
    uint8_t report[USB_STATUS_REPORT_SIZE + 8] = {};
    memset(report, 'A', sizeof(report));
    char message[USB_STATUS_TEXT_SIZE] = {};

    size_t written = usb_status_format_message(message, sizeof(message), report, sizeof(report));

    assert(written == USB_STATUS_REPORT_SIZE);
    assert(message[USB_STATUS_REPORT_SIZE] == '\0');
}

static void test_skips_control_bytes() {
    const uint8_t report[] = {'A', 0x01, '\t', 'B', 0x7f, 'C'};
    char message[USB_STATUS_TEXT_SIZE] = {};

    size_t written = usb_status_format_message(message, sizeof(message), report, sizeof(report));

    assert(written == strlen("A\tBC"));
    assert(strcmp(message, "A\tBC") == 0);
}

static void test_display_state_cycles_images() {
    DisplayState state;
    int mode = -2;

    assert(!state.take_pending_image_mode(&mode));

    state.select_next_image(5);
    assert(state.take_pending_image_mode(&mode));
    assert(mode == 0);
    assert(!state.take_pending_image_mode(&mode));

    state.select_previous_image(5);
    assert(state.take_pending_image_mode(&mode));
    assert(mode == 4);

    state.select_next_image(5);
    assert(state.take_pending_image_mode(&mode));
    assert(mode == 0);
}

static void test_display_state_status_resets_image_selection() {
    DisplayState state;
    int mode = -2;

    state.select_next_image(5);
    assert(state.take_pending_image_mode(&mode));

    state.show_status();
    assert(state.image_mode() == DisplayState::NO_IMAGE_MODE);
    assert(!state.take_pending_image_mode(&mode));
}

int main() {
    test_formats_hid_report_text();
    test_limits_hid_report_to_report_size();
    test_skips_control_bytes();
    test_display_state_cycles_images();
    test_display_state_status_resets_image_selection();
    return 0;
}
