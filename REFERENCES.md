# References & credits

This reverse engineering builds directly on prior community work. Huge thanks to:

- **[Jutta-Proto](https://github.com/Jutta-Proto)** — the canonical JURA protocol implementations:
  - [`protocol-cpp`](https://github.com/Jutta-Proto/protocol-cpp) — UART (V2) + the 0x5B codec.
  - [`protocol-bt-cpp`](https://github.com/Jutta-Proto/protocol-bt-cpp) — Bluetooth (TT214H), product/brew
    command structure, statistics, machine-file XML.
  - [`protocol-wifi-cpp`](https://github.com/Jutta-Proto/protocol-wifi-cpp) — the upstream this repo forks.
- **[COM8/esp32-jura](https://github.com/COM8/esp32-jura)** — original ESP32 UART + XMPP implementation;
  the closest existing analog to an ESP32 JURA bridge.
- **[mkalen/jura-smartconnect-wifi](https://github.com/mkalen/jura-smartconnect-wifi)** — the WiFi Smart
  Connect UDP discovery layer (TT237W version, machine name, `EF###` type, MAC).
- **[juramote](https://6xq.net/juramote/)** (PromyLOPh) — the older **V1** machine UART command set.
- **[Q42 "Hacking the coffee machine"](https://blog.q42.nl/hacking-the-coffee-machine-5802172b17c1)** and
  the coffeemakers.de forum — early V1 protocol work.

Tooling: [Ghidra](https://ghidra-sre.org/) (Xtensa decompilation), [jadx](https://github.com/skylot/jadx)
(APK), [esptool](https://github.com/espressif/esptool) (flash I/O),
[tenable/esp32_image_parser](https://github.com/tenable/esp32_image_parser) (partitions/NVS).

## Scope & ethics
This is interoperability / repairability research on **own hardware**. No JURA proprietary binaries
(firmware dump, decompiled firmware, the J.O.E. APK) are published here — only reverse-engineered
**protocol documentation**, consistent with the prior work above. Licensed GPL-3.0.
