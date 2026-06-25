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

int main() {
    test_formats_hid_report_text();
    test_limits_hid_report_to_report_size();
    test_skips_control_bytes();
    return 0;
}
