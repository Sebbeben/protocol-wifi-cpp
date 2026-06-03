# JURA WiFi Connect — protocol reverse engineering

Reverse engineering of the **JURA WiFi Connect** dongle (module **TT237W**) so a JURA coffee machine
can be controlled **locally** — without JURA's cloud — and integrated with Home Assistant / MQTT.

For a JURA machine to be controlled over WiFi, a [WiFi Connect](https://uk.jura.com/en/homeproducts/accessories/WiFi-Connect-24160)
dongle is required. It plugs into the machine's service port and bridges the machine's UART to WiFi.

![WiFi Connect dongle](ressources/images/wifi_dongle_case.png)

> **⚠️ Status: reverse-engineered, NOT yet hardware-verified.**
> The protocol documentation here was derived from **static analysis** of a dongle's firmware and the
> J.O.E. Android app. It has **not yet been confirmed against a live device** (no compatible machine on
> hand). Treat everything as a well-reasoned **working hypothesis** until a packet/UART capture confirms
> it. Corrections welcome.

## What's inside the dongle
- **SoC:** classic **ESP32-D0WD** (Xtensa LX6, dual-core) — *not* an ESP32-C3.
- **Running firmware:** a **custom JURA ESP-IDF app** that is **LAN-focused** — WiFi + a local TCP/UDP
  control server + the machine UART. It contains **no XMPP/cloud client**.
  - A stock **ESP-AT** firmware image is *also present in flash* (with cloud/OTA code) but it is
    **dormant — not booted**. Earlier write-ups that called the dongle "ESP-AT based" were reading that
    dormant image; see [`protocol/firmware-images.md`](protocol/firmware-images.md).
- It speaks the **JURA UART protocol** (9600 8N1, the "0x5B" transfer encoding) to the machine, and
  exposes control over **WiFi on the LAN** via its own `@H…` protocol.

## How control actually works (the key finding)
The dongle is **primarily a LAN device**. On the local network it runs a **TCP + UDP server on port
51515** speaking a line-based `@H…` command protocol; the actual machine commands are tunnelled over a
`*`-prefixed encoded channel. **The running firmware has no cloud client**, so for local control you
just talk to the dongle directly over the LAN — no cloud involved. (JURA's own cloud — Keycloak +
`joeapi.jura.com`, codename *pocketpilot* — is used by the phone app; how it reaches the dongle in this
firmware version is unconfirmed.)

→ Full details in **[`protocol/`](protocol/)**.

## Three ways to get local control
1. **Talk the dongle's own LAN protocol** (`protocol/wifi-local-protocol.md`) — no reflash, no cloud.
   Drive the stock dongle from Home Assistant over TCP/UDP 51515.
2. **Custom firmware** — reflash the dongle's ESP32 with firmware that speaks the JURA UART protocol and
   bridges to MQTT. (Developed/tested privately first; published here once it runs on real hardware.)
3. **Self-hosted cloud** — stand up your own server and DNS-redirect the dongle. Heaviest; documented in
   `protocol/cloud-architecture.md`.

## Repository layout
| Path | Contents |
|------|----------|
| `protocol/` | The reverse-engineered protocols: WiFi LAN control, UART/0x5B codec, cloud architecture |
| `ressources/` | Images |
| `REFERENCES.md` | Prior work this builds on (Jutta-Proto, COM8/esp32-jura, mkalen, juramote) |

> **Not published here:** the firmware dump, decompiled firmware, and the J.O.E. APK are JURA's
> proprietary material and are kept private. The DIY PCB/3D-case and the (untested) custom firmware live
> in a separate repo until verified.

## Credits
Builds on the excellent prior work of the [Jutta-Proto](https://github.com/Jutta-Proto) project,
[COM8/esp32-jura](https://github.com/COM8/esp32-jura), [mkalen/jura-smartconnect-wifi], and
[juramote](https://6xq.net/juramote/). See [`REFERENCES.md`](REFERENCES.md). Licensed GPL-3.0.
