# hidraw Status Display Guide

The firmware exposes a composite USB device:

- USB CDC for stdio.
- Vendor-defined HID IN/OUT for host status updates.

The HID interface uses 64-byte reports for small state updates and control commands. Each report starts with a 3-byte header followed by a payload:

| Byte | Field | Current value |
| --- | --- | --- |
| 0 | command | `0x01` update |
| 1 | target | address slot |
| 2 | length | payload byte length, `0..61` |
| 3..63 | payload | target-specific data |

Address slot targets:

| Target | Slot |
| --- | --- |
| `0x01` | legacy display text, stored as LAN1 IPv4 |
| `0x10` | LAN1 IPv4 |
| `0x11` | LAN1 IPv6 |
| `0x12` | LAN2 IPv4 |
| `0x13` | LAN2 IPv6 |

For address updates, the Pico treats the payload as printable ASCII, trims trailing NUL bytes, ignores carriage returns, and stores it in the selected slot. The startup Lenna image stays visible until any physical A/B/C button is pressed; that button press transitions to the four-slot status/IP screen. After that transition, later address updates render immediately. Unknown commands, unknown targets, truncated payloads, and payload lengths above 61 bytes are ignored.

The Pico also sends a HID IN report when the physical A/B/C button state changes:

| Byte | Field | Current value |
| --- | --- | --- |
| 0 | command | `0x02` state |
| 1 | target | `0x02` buttons |
| 2 | length | `0x01` |
| 3 | payload | pressed-button bitmask |

Button bitmask values:

| Bit | Button | GPIO |
| --- | --- | ---: |
| 0 | A | 12 |
| 1 | B | 13 |
| 2 | C | 14 |

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

Write one LAN1 IPv4 update report to the matching hidraw device:

```sh
python3 - <<'PY' /dev/hidrawX "IP: 192.0.2.10"
from pathlib import Path
import sys

path = Path(sys.argv[1])
payload = sys.argv[2].encode("ascii", errors="replace")[:61]
report = bytes([0x01, 0x10, len(payload)]) + payload
path.write_bytes(report.ljust(64, b"\0"))
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
payload = sys.argv[2].encode("ascii", errors="replace")[:61]
report = bytes([0x01, 0x10, len(payload)]) + payload
path.write_bytes(report.ljust(64, b"\0"))
PY
```

Reload udev rules after installing the rule:

```sh
sudo udevadm control --reload-rules
sudo udevadm trigger
```
