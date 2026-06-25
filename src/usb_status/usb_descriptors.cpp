#include "tusb.h"

#include "pico/unique_id.h"
#include "usb_status/usb_status.h"

namespace {

constexpr uint16_t USB_VID = 0x2e8a;
constexpr uint16_t USB_PID = 0x00a0;
constexpr uint16_t USB_BCD_DEVICE = 0x0100;

enum InterfaceNumber {
    ITF_NUM_CDC = 0,
    ITF_NUM_CDC_DATA,
    ITF_NUM_HID,
    ITF_NUM_TOTAL
};

enum StringId {
    STRID_LANGID = 0,
    STRID_MANUFACTURER,
    STRID_PRODUCT,
    STRID_SERIAL,
    STRID_CDC,
    STRID_HID
};

constexpr uint8_t EP_CDC_NOTIF = 0x81;
constexpr uint8_t EP_CDC_OUT = 0x02;
constexpr uint8_t EP_CDC_IN = 0x82;
constexpr uint8_t EP_HID_OUT = 0x03;
constexpr uint8_t EP_HID_IN = 0x83;

constexpr uint8_t CDC_NOTIF_SIZE = 8;
constexpr uint8_t CDC_DATA_SIZE = 64;
constexpr uint8_t HID_POLL_INTERVAL_MS = 10;

uint8_t const hid_report_descriptor[] = {
    TUD_HID_REPORT_DESC_GENERIC_INOUT(USB_STATUS_REPORT_SIZE)
};

constexpr uint16_t CONFIG_TOTAL_LENGTH = TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_HID_INOUT_DESC_LEN;

tusb_desc_device_t const device_descriptor = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = USB_VID,
    .idProduct = USB_PID,
    .bcdDevice = USB_BCD_DEVICE,
    .iManufacturer = STRID_MANUFACTURER,
    .iProduct = STRID_PRODUCT,
    .iSerialNumber = STRID_SERIAL,
    .bNumConfigurations = 1,
};

uint8_t const configuration_descriptor[] = {
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, STRID_LANGID, CONFIG_TOTAL_LENGTH, 0, 250),

    TUD_CDC_DESCRIPTOR(
        ITF_NUM_CDC,
        STRID_CDC,
        EP_CDC_NOTIF,
        CDC_NOTIF_SIZE,
        EP_CDC_OUT,
        EP_CDC_IN,
        CDC_DATA_SIZE),

    TUD_HID_INOUT_DESCRIPTOR(
        ITF_NUM_HID,
        STRID_HID,
        HID_ITF_PROTOCOL_NONE,
        sizeof(hid_report_descriptor),
        EP_HID_OUT,
        EP_HID_IN,
        USB_STATUS_REPORT_SIZE,
        HID_POLL_INTERVAL_MS)
};

char const *const string_descriptors[] = {
    nullptr,
    "Raspberry Pi",
    "Pico Inkpack Status",
    nullptr,
    "Board CDC",
    "Status HID"
};

uint16_t string_descriptor_buffer[33];
char serial_buffer[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1];

} // namespace

extern "C" uint8_t const *tud_descriptor_device_cb(void) {
    return reinterpret_cast<uint8_t const *>(&device_descriptor);
}

extern "C" uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance) {
    (void)instance;
    return hid_report_descriptor;
}

extern "C" uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;
    return configuration_descriptor;
}

extern "C" uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void)langid;

    size_t length = 0;
    if (index == STRID_LANGID) {
        string_descriptor_buffer[1] = 0x0409;
        length = 1;
    } else {
        if (index >= sizeof(string_descriptors) / sizeof(string_descriptors[0])) {
            return nullptr;
        }

        char const *value = string_descriptors[index];
        if (index == STRID_SERIAL) {
            if (serial_buffer[0] == '\0') {
                pico_get_unique_board_id_string(serial_buffer, sizeof(serial_buffer));
            }
            value = serial_buffer;
        }
        if (value == nullptr) {
            return nullptr;
        }

        for (; length < 32 && value[length] != '\0'; ++length) {
            string_descriptor_buffer[1 + length] = value[length];
        }
    }

    string_descriptor_buffer[0] = static_cast<uint16_t>((TUSB_DESC_STRING << 8) | (2 * length + 2));
    return string_descriptor_buffer;
}

extern "C" uint16_t tud_hid_get_report_cb(
    uint8_t instance,
    uint8_t report_id,
    hid_report_type_t report_type,
    uint8_t *buffer,
    uint16_t requested_length) {
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)requested_length;
    return 0;
}

extern "C" void tud_hid_set_report_cb(
    uint8_t instance,
    uint8_t report_id,
    hid_report_type_t report_type,
    uint8_t const *buffer,
    uint16_t buffer_size) {
    (void)instance;
    (void)report_id;
    if (report_type == HID_REPORT_TYPE_OUTPUT) {
        (void)usb_status_receive_report(buffer, buffer_size);
    }
}
