#include "app/screen_state.h"
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
    char addresses[USB_STATUS_IP_SLOT_COUNT][USB_STATUS_TEXT_SIZE] = {};

    assert(usb_status_receive_report(report, sizeof(report)));
    assert(usb_status_take_message(message, sizeof(message)));
    assert(strcmp(message, "IP: 192\nOK") == 0);
    assert(usb_status_take_network_status(addresses));
    assert(strcmp(addresses[0], "IP: 192\nOK") == 0);
    assert(strcmp(addresses[1], "-") == 0);
    assert(strcmp(addresses[2], "-") == 0);
    assert(strcmp(addresses[3], "-") == 0);
}

static void test_receives_network_address_slots() {
    const uint8_t lan1_v4[] = {
        USB_STATUS_COMMAND_UPDATE,
        USB_STATUS_TARGET_LAN1_IPV4,
        11,
        '1', '9', '2', '.', '0', '.', '2', '.', '1', '0', '1'
    };
    const uint8_t lan1_v6[] = {
        USB_STATUS_COMMAND_UPDATE,
        USB_STATUS_TARGET_LAN1_IPV6,
        13,
        '2', '0', '0', '1', ':', 'd', 'b', '8', ':', ':', '1', '0', '1'
    };
    const uint8_t lan2_v4[] = {
        USB_STATUS_COMMAND_UPDATE,
        USB_STATUS_TARGET_LAN2_IPV4,
        11,
        '1', '9', '2', '.', '0', '.', '2', '.', '2', '0', '2'
    };
    char addresses[USB_STATUS_IP_SLOT_COUNT][USB_STATUS_TEXT_SIZE] = {};

    assert(usb_status_receive_report(lan1_v4, sizeof(lan1_v4)));
    assert(usb_status_receive_report(lan1_v6, sizeof(lan1_v6)));
    assert(usb_status_receive_report(lan2_v4, sizeof(lan2_v4)));
    assert(usb_status_take_network_status(addresses));
    assert(strcmp(addresses[0], "192.0.2.101") == 0);
    assert(strcmp(addresses[1], "2001:db8::101") == 0);
    assert(strcmp(addresses[2], "192.0.2.202") == 0);
    assert(strcmp(addresses[3], "-") == 0);
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

static void test_screen_transitions_from_startup_on_button_press() {
    ScreenState state;

    assert(state.is_showing_startup_image());
    assert(!state.handle_button_press(0));
    assert(state.is_showing_startup_image());
    assert(state.handle_button_press(USB_STATUS_BUTTON_B_MASK));
    assert(!state.is_showing_startup_image());
    assert(state.is_showing_network_status());
}

static void test_screen_rings_between_network_and_uptime_with_a_and_c() {
    ScreenState state;

    assert(state.handle_button_press(USB_STATUS_BUTTON_A_MASK));
    assert(state.is_showing_network_status());
    assert(state.handle_button_press(USB_STATUS_BUTTON_C_MASK));
    assert(state.is_showing_uptime());
    assert(state.handle_button_press(USB_STATUS_BUTTON_C_MASK));
    assert(state.is_showing_network_status());
    assert(state.handle_button_press(USB_STATUS_BUTTON_A_MASK));
    assert(state.is_showing_uptime());
    assert(state.handle_button_press(USB_STATUS_BUTTON_A_MASK));
    assert(state.is_showing_network_status());
}

static void test_screen_ignores_b_between_network_and_uptime() {
    ScreenState state;

    assert(state.handle_button_press(USB_STATUS_BUTTON_A_MASK));
    assert(state.is_showing_network_status());
    assert(!state.handle_button_press(USB_STATUS_BUTTON_B_MASK));
    assert(state.is_showing_network_status());
}

int main() {
    test_formats_hid_report_text();
    test_limits_hid_payload_to_payload_size();
    test_skips_control_bytes();
    test_receives_display_text_command_report();
    test_receives_network_address_slots();
    test_ignores_unknown_command_report();
    test_ignores_truncated_payload_report();
    test_builds_button_state_report();
    test_rejects_small_button_report_buffer();
    test_screen_transitions_from_startup_on_button_press();
    test_screen_rings_between_network_and_uptime_with_a_and_c();
    test_screen_ignores_b_between_network_and_uptime();
    return 0;
}
