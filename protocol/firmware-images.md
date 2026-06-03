# Firmware images: what actually runs

The dongle's flash contains **two complete, different app firmwares**. Knowing which one boots matters —
it changes what the dongle actually does.

> **⚠️ Reverse-engineered from a firmware dump; not yet hardware-verified.**

## The two app images
| Image | Flash offset | Booted? | What it is |
|-------|-------------|---------|------------|
| **image-A** | `0x030000` (`ota_0`) | **YES** | A **custom JURA ESP-IDF app**. WiFi + the local `@H…` TCP/UDP control servers + the machine UART. **No XMPP, no cloud client** — it's a LAN device. |
| **image-B** | `0x100000` | **NO** | A stock **ESP-AT** firmware (XMPP, `+CIPUPDATE`/`download_rom` OTA to `iot.espressif.cn`, example TLS CA). **Dormant — never booted.** |

## Why image-A is the one that runs
- The partition table (`@0x8000`) places `ota_0` at `0x030000`; `otadata` selects it (seq=1) → the stock
  2nd-stage bootloader boots **image-A**.
- image-A is a complete standalone ESP-IDF app (valid entry, FreeRTOS, `esp_wifi`, lwIP, `app_main`).

## Why image-B is dormant
- image-B (~1.1 MB) physically **overflows the `ota_0` partition** (into the `ota_1` region), so it
  **cannot be booted via the partition table**.
- image-A contains **no loader** for it (no reference to offset `0x100000`, no jump to image-B's entry).
- A 0xFF gap separates the two → independently flashed. image-B sits at **ESP-AT's native `0x100000`
  app offset**, i.e. it's the module's **original stock ESP-AT firmware**, left in flash when JURA
  flashed their own app (image-A) and made it the active boot image.

## Consequence
The live dongle has **no ESP-AT and no cloud client** — it is controlled over the **LAN** via its `@H…`
protocol (see [`wifi-local-protocol.md`](wifi-local-protocol.md)). Any "ESP-AT / `iot.espressif.cn` /
no-TLS-verify" observations belong to the **dormant** image-B and do **not** describe the running dongle.
