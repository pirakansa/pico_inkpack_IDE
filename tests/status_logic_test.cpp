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

int main() {
    test_formats_hid_report_text();
    test_limits_hid_payload_to_payload_size();
    test_skips_control_bytes();
    test_receives_display_text_command_report();
    test_ignores_unknown_command_report();
    test_ignores_truncated_payload_report();
    return 0;
}
