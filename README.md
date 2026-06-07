# JURA WiFi Protocol
`C++` JURA WiFi protocol implementation for controlling a JURA coffee maker over a WiFi connection.

**Currently still work in progress since I do not own a compatible JURA coffee maker for the WiFi Connect dongle.**

For a device to be able to connect to an JURA coffee maker via WiFi, a [WiFi Connect](https://uk.jura.com/en/homeproducts/accessories/WiFi-Connect-24160) dongle is required.

![WiFi Connect dongle](ressources/images/wifi_dongle_case.png)

## Dumping the Firmware
![ESP32 inside the WiFi Connect dongle](ressources/images/wifi_dongle_esp32.png)

## Reverse-engineered protocol documentation
This adds reverse-engineered documentation of the WiFi Connect dongle's protocols — see
[`protocol/`](protocol/):

- **Which firmware runs** — the booted image is a **custom JURA LAN app** (no cloud client); a stock
  **ESP-AT** image is present in flash but **dormant**. ([`protocol/firmware-images.md`](protocol/firmware-images.md))
- **WiFi LAN control protocol** — the dongle's own TCP/UDP server on port **51515** (`@H…` command set
  + a `*`-prefixed encoded coffee-machine passthrough). Lets the stock dongle be controlled locally,
  no cloud. ([`protocol/wifi-local-protocol.md`](protocol/wifi-local-protocol.md))
- **JURA UART protocol** — 9600 8N1, the keyless 0x5B transfer encoding, V2 handshake + older V1
  vocabulary. ([`protocol/uart-protocol.md`](protocol/uart-protocol.md))
- **Cloud architecture** — Keycloak / `joeapi.jura.com` / *pocketpilot*, observed from the J.O.E. app.
  ([`protocol/cloud-architecture.md`](protocol/cloud-architecture.md))

- **`*`-channel codec** — a compiled, self-testing C++ reference implementation of the WiFi obfuscation
  (the same shuffle as JURA Bluetooth). ([`src/wifi/ByteEncDecoder.cpp`](src/wifi/ByteEncDecoder.cpp))

> ✅ **The WiFi `@H` LAN protocol is now hardware-verified** (2026-06-06) on a real TT237W dongle —
> transport, the `@HP:` handshake, the `@H…` identity/version commands, and the `*`-channel obfuscation
> (decodes captured frames to exact ASCII). Still hypothesis pending a live machine: exact `@HP:` field
> semantics, `@HR`/`@HB` details, and the live `*` machine command stream. UART/cloud docs remain static analysis.

Builds on prior work by Jutta-Proto, [COM8/esp32-jura](https://github.com/COM8/esp32-jura),
[mkalen/jura-smartconnect-wifi](https://github.com/mkalen/jura-smartconnect-wifi), and
[juramote](https://6xq.net/juramote/) — see [REFERENCES.md](REFERENCES.md).