# Build and Flash Guide

## Prerequisites

Use the dev container when possible. It provides the Pico SDK toolchain and the `vorbere` task runner expected by this repository.

External source trees are managed by `mani.yaml`:

- `pico-sdk`
- `pimoroni-pico`

Prepare dependencies with:

```sh
vorbere run setup
```

## Validate

Run static checks:

```sh
vorbere run check
```

Run host tests:

```sh
vorbere run test
```

Render e-paper screen previews:

```sh
vorbere run ui-preview
```

## Build

Build for Raspberry Pi Pico W:

```sh
vorbere run build
```

Build for Raspberry Pi Pico 2 W:

```sh
vorbere run build-pico2
```

## Flash

After a successful Pico W build, copy the generated UF2 to the mounted boot volume:

```sh
cp build/picopro.uf2 /media/$USER/RPI-RP2/
```

For Pico 2 W builds:

```sh
cp build-pico2/picopro.uf2 /media/$USER/RPI-RP2/
```

The mount path can vary by host operating system.
