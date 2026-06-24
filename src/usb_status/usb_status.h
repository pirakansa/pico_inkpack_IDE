#ifndef USB_STATUS_H
#define USB_STATUS_H

#include <stddef.h>
#include <stdint.h>

constexpr size_t USB_STATUS_REPORT_SIZE = 64;
constexpr size_t USB_STATUS_TEXT_SIZE = USB_STATUS_REPORT_SIZE + 1;

bool usb_status_take_message(char *buffer, size_t buffer_size);
size_t usb_status_format_message(char *buffer, size_t buffer_size, const uint8_t *data, uint16_t length);
void usb_status_receive_report(const uint8_t *data, uint16_t length);

#endif
