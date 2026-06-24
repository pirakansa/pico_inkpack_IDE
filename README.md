# pico_inkpack_IDE

Firmware experiments for Raspberry Pi Pico W and Pimoroni e-paper/Inky hardware.

## Quick Start

Use the dev container when possible. It includes the Pico SDK toolchain and `vorbere`.

```sh
vorbere run setup
vorbere run check
vorbere run test
vorbere run build
```

The default build target is `pico_w` and writes firmware artifacts under `build/`.

## Documentation

- [Documentation index](docs/README.md)
- [Module overview](docs/module-overview.md)
- [Build and flash guide](docs/user-guides/build-and-flash.md)
- [hidraw status display guide](docs/user-guides/hidraw-status.md)
- [Image data reference](docs/reference/image-data.md)
