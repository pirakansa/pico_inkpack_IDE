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

static void test_limits_hid_payload_to_payload_size() {
    uint8_t report[USB_STATUS_REPORT_SIZE + 8] = {};
    memset(report, 'A', sizeof(report));
    char message[USB_STATUS_TEXT_SIZE] = {};

    size_t written = usb_status_format_message(message, sizeof(message), report, sizeof(report));

    assert(written == USB_STATUS_REPORT_PAYLOAD_SIZE);
    assert(message[USB_STATUS_REPORT_PAYLOAD_SIZE] == '\0');
}

static void test_skips_control_bytes() {
    const uint8_t report[] = {'A', 0x01, '\t', 'B', 0x7f, 'C'};
    char message[USB_STATUS_TEXT_SIZE] = {};

    size_t written = usb_status_format_message(message, sizeof(message), report, sizeof(report));

    assert(written == strlen("A\tBC"));
    assert(strcmp(message, "A\tBC") == 0);
}

static void test_receives_display_text_command_report() {
    const uint8_t report[] = {
        USB_STATUS_COMMAND_UPDATE,
        USB_STATUS_TARGET_DISPLAY_TEXT,
        11,
        'I', 'P', ':', ' ', '1', '9', '2', '\r', '\n', 'O', 'K'
    };
    char message[USB_STATUS_TEXT_SIZE] = {};

    assert(usb_status_receive_report(report, sizeof(report)));
    assert(usb_status_take_message(message, sizeof(message)));
    assert(strcmp(message, "IP: 192\nOK") == 0);
}

static void test_ignores_unknown_command_report() {
    const uint8_t report[] = {
        0xff,
        USB_STATUS_TARGET_DISPLAY_TEXT,
        5,
        'h', 'e', 'l', 'l', 'o'
    };
    char message[USB_STATUS_TEXT_SIZE] = {};

    assert(!usb_status_receive_report(report, sizeof(report)));
    assert(!usb_status_take_message(message, sizeof(message)));
}

static void test_ignores_truncated_payload_report() {
    const uint8_t report[] = {
        USB_STATUS_COMMAND_UPDATE,
        USB_STATUS_TARGET_DISPLAY_TEXT,
        6,
        'h', 'e', 'l', 'l', 'o'
    };
    char message[USB_STATUS_TEXT_SIZE] = {};

    assert(!usb_status_receive_report(report, sizeof(report)));
    assert(!usb_status_take_message(message, sizeof(message)));
}

static void test_builds_button_state_report() {
    uint8_t report[USB_STATUS_REPORT_SIZE] = {};

    size_t written = usb_status_build_button_report(
        report,
        sizeof(report),
        USB_STATUS_BUTTON_A_MASK | USB_STATUS_BUTTON_C_MASK | 0x80);

    assert(written == USB_STATUS_REPORT_SIZE);
    assert(report[0] == USB_STATUS_COMMAND_STATE);
    assert(report[1] == USB_STATUS_TARGET_BUTTONS);
    assert(report[2] == 1);
    assert(report[USB_STATUS_REPORT_HEADER_SIZE] == (
        USB_STATUS_BUTTON_A_MASK | USB_STATUS_BUTTON_C_MASK));
    assert(report[USB_STATUS_REPORT_HEADER_SIZE + 1] == 0);
}

static void test_rejects_small_button_report_buffer() {
    uint8_t report[USB_STATUS_REPORT_SIZE - 1] = {};

    size_t written = usb_status_build_button_report(
        report,
        sizeof(report),
        USB_STATUS_BUTTON_A_MASK);

    assert(written == 0);
}

int main() {
    test_formats_hid_report_text();
    test_limits_hid_payload_to_payload_size();
    test_skips_control_bytes();
    test_receives_display_text_command_report();
    test_ignores_unknown_command_report();
    test_ignores_truncated_payload_report();
    test_builds_button_state_report();
    test_rejects_small_button_report_buffer();
    return 0;
}
