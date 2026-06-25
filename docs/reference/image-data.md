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

The current display size is 296 by 128 pixels, so each retained framebuffer contains 4,736 bytes.

## Host Preview

The compiled image resources are currently retained for reference, but the firmware no longer displays them.
