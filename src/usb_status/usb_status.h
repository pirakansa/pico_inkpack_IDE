#ifndef USB_STATUS_H
#define USB_STATUS_H

#include <stddef.h>
#include <stdint.h>

constexpr size_t USB_STATUS_REPORT_SIZE = 64;
constexpr size_t USB_STATUS_REPORT_HEADER_SIZE = 3;
constexpr size_t USB_STATUS_REPORT_PAYLOAD_SIZE = USB_STATUS_REPORT_SIZE - USB_STATUS_REPORT_HEADER_SIZE;
constexpr size_t USB_STATUS_TEXT_SIZE = USB_STATUS_REPORT_PAYLOAD_SIZE + 1;

constexpr uint8_t USB_STATUS_COMMAND_UPDATE = 0x01;
constexpr uint8_t USB_STATUS_TARGET_DISPLAY_TEXT = 0x01;

bool usb_status_take_message(char *buffer, size_t buffer_size);
size_t usb_status_format_message(char *buffer, size_t buffer_size, const uint8_t *data, uint16_t length);
bool usb_status_receive_report(const uint8_t *data, uint16_t length);

#endif
