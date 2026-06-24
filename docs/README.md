# Documentation Index

This page is the entry point for repository documentation.

## Reading Guide

| If you want to understand... | Start here |
| --- | --- |
| the current firmware structure | [module-overview.md](module-overview.md) |
| how to set up and build the firmware | [user-guides/build-and-flash.md](user-guides/build-and-flash.md) |
| how to write host status text over USB hidraw | [user-guides/hidraw-status.md](user-guides/hidraw-status.md) |
| how image resources are stored and regenerated | [reference/image-data.md](reference/image-data.md) |

## Document Map

```mermaid
flowchart TD
    A[Documentation Index] --> B[Module Overview]
    A --> C[Build and Flash Guide]
    A --> D[Image Data Reference]
    A --> E[hidraw Status Display Guide]
    B --> C
    B --> D
    B --> E
    C --> D
```

## Suggested Reading Order

1. Read [module-overview.md](module-overview.md) for the implemented source layout.
2. Read [user-guides/build-and-flash.md](user-guides/build-and-flash.md) for dependency setup and build commands.
3. Read [user-guides/hidraw-status.md](user-guides/hidraw-status.md) to send host status text to the display.
4. Read [reference/image-data.md](reference/image-data.md) before replacing bundled image resources.
