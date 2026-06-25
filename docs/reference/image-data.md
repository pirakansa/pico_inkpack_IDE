# Image Data Reference

The e-paper images are compiled into `src/epaper/ImageData.cpp`.
Public declarations live in `src/epaper/ImageData.h`.

## Resource Location

Source image assets are stored under `src/epaper/res/`.
Keep future bitmap or editor source files there when they are only used by the e-paper module.

## Conversion Tool

`tools/bmp2Monopic.py` converts a monochrome BMP into byte-oriented C initializer data.
It requires Python and OpenCV.

Example:

```sh
python3 tools/bmp2Monopic.py src/epaper/res/Lenna.bmp
```

The current manual formatting pipeline is:

```sh
python3 tools/bmp2Monopic.py src/epaper/res/Lenna.bmp | sed s/\'//g | sed s/]/\\n}\\;/g | sed s/\\[/{\\n/g
```

Paste the generated initializer body into `src/epaper/ImageData.cpp` when intentionally replacing an image.

## Data Contract

The current display size is 296 by 128 pixels, so each framebuffer contains 4,736 bytes.

## Firmware Usage

The firmware displays `lennaImage` during startup and keeps it visible until any physical A/B/C button is pressed.

## Host Preview

Render the compiled images and screen layout previews as PPM files:

```sh
vorbere run ui-preview
```

The address screen preview is written to `build/ui-preview/address-screen.ppm`.
The USB uptime screen preview is written to `build/ui-preview/uptime-screen.ppm`.
