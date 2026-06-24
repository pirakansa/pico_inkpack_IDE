# Module Overview

This document describes the current repository layout. Code and passing validation are the source of truth.

## Source Layout

- `src/main.cpp`: Pico W startup, Pimoroni e-paper setup, button handling, Wi-Fi scan trigger, and image selection.
- `src/epaper/ImageData.c`: compiled 1-bit framebuffers for the bundled images.
- `src/epaper/ImageData.h`: public image declarations shared by firmware and host preview code.
- `src/epaper/res/`: source bitmap and editor assets used to regenerate image data.
- `src/epaper/preview/`: host-side PPM preview renderer for the compiled image data.
- `tools/`: maintenance scripts, including bitmap conversion.

External dependencies stay at the repository root because they are managed by `mani.yaml` and ignored by Git:

- `pico-sdk`
- `pimoroni-pico`

## Runtime Flow

1. Initialize stdio.
2. Initialize the UC8151 e-paper display and 1-bit graphics buffer.
3. Render a startup message.
4. Use buttons A and C to cycle through compiled image framebuffers.

## Validation

The repository validation entry points are:

- `vorbere run check`: static repository text checks.
- `vorbere run test`: C host tests and e-paper preview generation.
- `vorbere run ui-preview`: render preview output under `build/ui-preview/`.
- `vorbere run build`: Pico W firmware build.
- `vorbere run build-pico2`: Pico 2 W firmware build.
