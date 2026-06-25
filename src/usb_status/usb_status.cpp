#include "usb_status/usb_status.h"

#include <ctype.h>
#include <string.h>

static char pending_message[USB_STATUS_TEXT_SIZE] = {};
static bool pending_message_available = false;
static char network_addresses[USB_STATUS_IP_SLOT_COUNT][USB_STATUS_TEXT_SIZE] = {
    "-",
    "-",
    "-",
    "-"
};
static bool network_status_available = false;

static bool usb_status_target_to_ip_slot(uint8_t target, size_t *slot) {
    switch (target) {
    case USB_STATUS_TARGET_DISPLAY_TEXT:
    case USB_STATUS_TARGET_LAN1_IPV4:
        *slot = 0;
        return true;
    case USB_STATUS_TARGET_LAN1_IPV6:
        *slot = 1;
        return true;
    case USB_STATUS_TARGET_LAN2_IPV4:
        *slot = 2;
        return true;
    case USB_STATUS_TARGET_LAN2_IPV6:
        *slot = 3;
        return true;
    default:
        return false;
    }
}

bool usb_status_take_message(char *buffer, size_t buffer_size) {
    if (!pending_message_available || buffer_size == 0) {
        return false;
    }

    strncpy(buffer, pending_message, buffer_size - 1);
    buffer[buffer_size - 1] = '\0';
    pending_message_available = false;
    return true;
}

bool usb_status_take_network_status(char addresses[USB_STATUS_IP_SLOT_COUNT][USB_STATUS_TEXT_SIZE]) {
    if (!network_status_available) {
        return false;
    }

    for (size_t slot = 0; slot < USB_STATUS_IP_SLOT_COUNT; ++slot) {
        strncpy(addresses[slot], network_addresses[slot], USB_STATUS_TEXT_SIZE - 1);
        addresses[slot][USB_STATUS_TEXT_SIZE - 1] = '\0';
    }
    network_status_available = false;
    return true;
}

size_t usb_status_format_message(char *buffer, size_t buffer_size, const uint8_t *data, uint16_t length) {
    if (buffer_size == 0) {
        return 0;
    }

    size_t message_length = length;
    if (message_length > USB_STATUS_REPORT_PAYLOAD_SIZE) {
        message_length = USB_STATUS_REPORT_PAYLOAD_SIZE;
    }

    while (message_length > 0 && data[message_length - 1] == '\0') {
        --message_length;
    }

    size_t write_index = 0;
    for (size_t read_index = 0; read_index < message_length && write_index < buffer_size - 1; ++read_index) {
        unsigned char c = data[read_index];
        if (c == '\r') {
            continue;
        }
        if (c == '\n' || c == '\t' || isprint(c)) {
            buffer[write_index++] = (char)c;
        }
    }
    buffer[write_index] = '\0';
    return write_index;
}

bool usb_status_receive_report(const uint8_t *data, uint16_t length) {
    if (length < USB_STATUS_REPORT_HEADER_SIZE) {
        return false;
    }

    uint8_t command = data[0];
    uint8_t target = data[1];
    uint8_t payload_length = data[2];
    if (payload_length > USB_STATUS_REPORT_PAYLOAD_SIZE ||
        payload_length > length - USB_STATUS_REPORT_HEADER_SIZE) {
        return false;
    }

    size_t ip_slot = 0;
    if (command != USB_STATUS_COMMAND_UPDATE || !usb_status_target_to_ip_slot(target, &ip_slot)) {
        return false;
    }

    size_t formatted_length = usb_status_format_message(
        network_addresses[ip_slot],
        sizeof(network_addresses[ip_slot]),
        data + USB_STATUS_REPORT_HEADER_SIZE,
        payload_length);
    if (formatted_length == 0) {
        strcpy(network_addresses[ip_slot], "-");
    }
    network_status_available = true;

    if (target == USB_STATUS_TARGET_DISPLAY_TEXT) {
        strncpy(pending_message, network_addresses[ip_slot], sizeof(pending_message) - 1);
        pending_message[sizeof(pending_message) - 1] = '\0';
        pending_message_available = true;
    }
    return true;
}

size_t usb_status_build_button_report(uint8_t *buffer, size_t buffer_size, uint8_t button_mask) {
    if (buffer_size < USB_STATUS_REPORT_SIZE) {
        return 0;
    }

    memset(buffer, 0, USB_STATUS_REPORT_SIZE);
    buffer[0] = USB_STATUS_COMMAND_STATE;
    buffer[1] = USB_STATUS_TARGET_BUTTONS;
    buffer[2] = 1;
    buffer[USB_STATUS_REPORT_HEADER_SIZE] = button_mask & (
        USB_STATUS_BUTTON_A_MASK |
        USB_STATUS_BUTTON_B_MASK |
        USB_STATUS_BUTTON_C_MASK);
    return USB_STATUS_REPORT_SIZE;
}
