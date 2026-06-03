# WiFi Connect — local LAN control protocol

How the JURA WiFi Connect dongle is controlled **on the local network**, reverse-engineered from the
dongle's firmware. This is the basis for cloud-free local control (and matches the discovery layer in
[mkalen/jura-smartconnect-wifi]).

> **⚠️ Reverse-engineered, not yet hardware-verified.** Confirm against a live capture before relying on
> exact field orders / the `*`-channel key derivation.

## Transport
- On start-up the dongle opens **two servers, both on port `0xC93B` = 51515**:
  - a **TCP** control server (`socket`/`bind`/`listen`/`accept`),
  - a **UDP** socket used for broadcast **discovery**.
- It also offers a setup **SoftAP** whose SSID is `"CoffeeMachine_" + <MAC-derived hex> + <XOR-0xFF checksum>`.
- The protocol is **line-based ASCII, terminated with `\r\n`** (2 KB RX buffer). Responses to `@HX`
  queries come back lowercase as `@hx:…`. On session close the dongle sends `@TS:00\r\n`.

## Command dispatch
After each `\r\n`-terminated line the dongle:
1. If the line starts with **`*`** → routes it to the encoded **coffee-machine passthrough** (below).
2. Otherwise expects a 4-byte gate command **`@HP:`** first (a handshake/provisioning step); until that
   succeeds other commands are refused.
3. Otherwise matches the first 3 characters against a **12-entry command table** and calls the handler.

## Command set — the `@H…` family
| Command | Meaning (response) |
|---------|--------------------|
| `@HI` | identity / signature → `@hi:SIGNATURE…` |
| `@HL` | loader + module version → `@hl:ESP32LDRSIM TT237W V05.26` |
| `@HY` | module type/version → `@hy:TT237W V05.26` |
| `@HW…` | **set WiFi station config** (SSID/credentials → connect) |
| `@HR<reg>` | **read status register** (e.g. `05` ≈ signal level, `8F` ≈ a custom pin) → `@hr:…` |
| `@HT<n>` | **control**: `0`/`1` = a GPIO/relay off/on; `2` = disconnect; `3` = **reboot** |
| `@HP:<f1>,<f2>,…` | **provisioning / gate** (comma-separated fields ≤6 chars) → `@hp4:ok` / `@hp5:` error |
| `@HU` | **update trigger** → `@hu:000`/status, `@hu:wait`/`@hu:busy`/`@hu:abort` |
| `@HB` | **discovery/status beacon** (also UDP-broadcasts device info) |
| `@HO` | **OTA begin** (validate partition, `esp_ota_begin`) → `@ho:ok` / `@ho:error` |
| `@HD<off><len><hex>` | **OTA data block** (hex-encoded, de-obfuscated, ≤512 B, 4-byte aligned) |
| `@HE` | **OTA end** (`esp_ota_end` + set boot partition) → `@he:ok` / `@he:error` |

Asynchronous status pushes use `@ha:…` (e.g. `@ha:03,2%01X`, `@ha:05,2`).

## The `*` channel — coffee-machine passthrough
Lines beginning with `*` carry the **actual machine commands** (brew/status — see `uart-protocol.md`)
wrapped in a keyed nibble obfuscation:
- Optional **ESC (`0x1B`) escaping**: a `0x1B` means "clear the high bit of the next byte".
- The first payload byte's high/low nibbles are the **per-message key** (`k1`, `k2`).
- Each data byte is split into nibbles; each nibble `x` at position `pos` is decoded as
  **`out = (x − pos − k1) & 0x0F`** and recombined `hi<<4 | lo`.
- The decoded bytes are the JURA UART command stream (`TY:`, `FA:`, `FN:`, the `@T1/@T2/@T3` handshake…).

This is distinct from the machine-side 4-bytes-per-byte `0x5B` UART encoding (`uart-protocol.md`); the
`*` channel is the WiFi transport's own layer on top.

## Using it for local control (untested recipe)
1. TCP-connect to `dongle_ip:51515`.
2. Complete the `@HP:` handshake, then `@HY` to identify the module, `@HI`/`@HL` for signature/version.
3. Set WiFi with `@HW`; read status with `@HR`; reboot with `@HT3`.
4. Send machine commands over the `*` channel; OTA the dongle with `@HO`/`@HD`/`@HE`.

**Still to confirm on hardware:** exact `@HP:` field order/meaning, the precise `*` key derivation, and
the UDP discovery payload format. A single Wireshark capture of the J.O.E. app controlling a real dongle
will nail these down.

[mkalen/jura-smartconnect-wifi]: https://github.com/mkalen/jura-smartconnect-wifi
