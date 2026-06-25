#include "usb_status/usb_status.h"

#include <ctype.h>
#include <string.h>

static char pending_message[USB_STATUS_TEXT_SIZE] = {};
static bool pending_message_available = false;

bool usb_status_take_message(char *buffer, size_t buffer_size) {
    if (!pending_message_available || buffer_size == 0) {
        return false;
    }

    strncpy(buffer, pending_message, buffer_size - 1);
    buffer[buffer_size - 1] = '\0';
    pending_message_available = false;
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

    if (command != USB_STATUS_COMMAND_UPDATE || target != USB_STATUS_TARGET_DISPLAY_TEXT) {
        return false;
    }

    usb_status_format_message(
        pending_message,
        sizeof(pending_message),
        data + USB_STATUS_REPORT_HEADER_SIZE,
        payload_length);
    pending_message_available = true;
    return true;
}
