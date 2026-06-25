# Module Overview

This document describes the current repository layout. Code and passing validation are the source of truth.

## Source Layout

- `src/main.cpp`: Pico W startup, TinyUSB polling, and high-level orchestration.
- `src/display/`: Pimoroni e-paper rendering for startup images and status text.
- `src/epaper/ImageData.cpp`: compiled 1-bit framebuffers for the bundled images.
- `src/epaper/ImageData.h`: public image declarations for the compiled framebuffers.
- `src/epaper/res/`: source bitmap and editor assets used to regenerate image data.
- `src/epaper/preview/`: host-side PPM preview renderer for the compiled image data.
- `src/tusb_config.h`: TinyUSB device class configuration for CDC plus vendor HID.
- `src/usb_status/`: USB descriptors, HID OUT report handling, and host-provided status text normalization.
- `tools/`: maintenance scripts, including bitmap conversion.

External dependencies stay at the repository root because they are managed by `mani.yaml` and ignored by Git:

- `pico-sdk`
- `pimoroni-pico`

## Runtime Flow

1. Initialize TinyUSB and stdio.
2. Initialize the UC8151 e-paper display and 1-bit graphics buffer.
3. Render the Lenna image as the startup screen.
4. Poll TinyUSB in the main loop.
5. When a 64-byte HID OUT report arrives, normalize printable status text and render it on the e-paper display.

## Validation

The repository validation entry points are:

- `vorbere run check`: static repository text checks.
- `vorbere run test`: C++ host tests.
- `vorbere run build`: Pico W firmware build.
- `vorbere run build-pico2`: Pico 2 W firmware build.
