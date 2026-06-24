# hidraw Status Display Guide

The firmware exposes a composite USB device:

- USB CDC for stdio.
- Vendor-defined HID IN/OUT for host status updates.

The HID interface uses a 64-byte report. The Pico treats each OUT report as printable ASCII status text, trims trailing NUL bytes, ignores carriage returns, and displays the result on the e-paper screen.

## Device Match

The current USB identifiers are:

- Vendor ID: `2e8a`
- Product ID: `00a0`
- Product string: `Pico Inkpack Status`
- HID interface string: `Status HID`

On Linux, inspect the enumerated device with:

```sh
udevadm info -a -n /dev/hidrawX
```

## Manual Write

Write exactly one 64-byte report to the matching hidraw device:

```sh
python3 - <<'PY' /dev/hidrawX "IP: 192.0.2.10"
from pathlib import Path
import sys

path = Path(sys.argv[1])
message = sys.argv[2].encode("ascii", errors="replace")[:64]
path.write_bytes(message.ljust(64, b"\0"))
PY
```

Replace `/dev/hidrawX` with the device created for the `Status HID` interface.

## udev Hook

A udev rule can run a host script when the HID interface appears:

```udev
ACTION=="add", SUBSYSTEM=="hidraw", ATTRS{idVendor}=="2e8a", ATTRS{idProduct}=="00a0", RUN+="/usr/local/bin/pico-inkpack-status %E{DEVNAME}"
```

Example script:

```sh
#!/bin/sh
set -eu

device="$1"
ip="$(hostname -I | awk '{print $1}')"
status="IP: ${ip:-unknown}"

python3 - "$device" "$status" <<'PY'
from pathlib import Path
import sys

path = Path(sys.argv[1])
message = sys.argv[2].encode("ascii", errors="replace")[:64]
path.write_bytes(message.ljust(64, b"\0"))
PY
```

Reload udev rules after installing the rule:

```sh
sudo udevadm control --reload-rules
sudo udevadm trigger
```
