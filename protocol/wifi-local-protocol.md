# WiFi Connect — local LAN control protocol

How the JURA WiFi Connect dongle is controlled **on the local network**, reverse-engineered from the
dongle's firmware. This is the basis for cloud-free local control (and matches the discovery layer in
[mkalen/jura-smartconnect-wifi]).

> **✅ Hardware-verified (2026-06-06).** The transport, the `@HP:` handshake, the `@H…` identity/version
> commands, and the `*`-channel obfuscation are confirmed on a real TT237W dongle. Items still needing a
> coffee machine attached are called out at the end.

## Transport
- On start-up the dongle opens **two servers, both on port `0xC93B` = 51515**:
  - a **TCP** control server (`socket`/`bind`/`listen`/`accept`),
  - a **UDP** socket used for broadcast **discovery**.
- It also offers a setup **SoftAP**. Observed SSID is **`WiFiFrog_<UPPER-HEX-MAC>`** (e.g.
  `WiFiFrog_3C8A1F18CB19`, AP BSSID = STA MAC + 1), **open** auth, with the dongle as gateway at
  **`192.168.24.1/24`**. (A `"CoffeeMachine_…"` builder also exists in firmware — model/variant dependent.)
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

## The `*` channel — coffee-machine passthrough (obfuscation **solved**)
Lines beginning with `*` carry the **actual machine commands** (brew/status — see `uart-protocol.md`)
wrapped in a keyed, position-dependent nibble shuffle. It is the **same algorithm JURA uses on Bluetooth**
— see [Jutta-Proto/protocol-bt-cpp] `src/bt/ByteEncDecoder.cpp` (`shuffle` / `encDecBytes`); only the
16-entry S-box tables differ between products. The transform is a **perfect involution** (the same routine
encodes and decodes).

Frame: `*` + key byte + obfuscated payload + `\r\n`, with optional **ESC (`0x1B`) escaping** (a `0x1B`
means "clear the high bit of the next byte"; the escape set is `0A 0D 2A 2B 26 1B 00`). The key byte's
high/low nibbles are `k1`,`k2`. Each plaintext byte is split into nibbles; for nibble `x` at running
position `pos` (0,1,2,… across the message):

```
i5 = pos >> 4
t1 = A[(x  + pos + k1) % 16]
t2 = B[(t1 + k2  + i5 - pos - k1) % 16]
t3 = A[(t2 + k1  + pos - k2 - i5) % 16]
out = (t3 - pos - k1) % 16          # all arithmetic mod 256 then % 16
```

WiFi-Connect (TT237W) S-box tables, read from the dongle firmware (`A`/`B` for the `*` channel):
```
A = [ 1, 0, 3, 2,15,14, 8,10, 6,13, 7,12,11, 9, 5, 4]
B = [ 9,12, 6,11,10,15, 2,14,13, 0, 4, 3, 1, 8, 7, 5]
```
(Two further table pairs exist for other framings; the `*` channel uses the pair above.) Verified: decoding
a captured `@hy` reply frame yields exactly `@hy:TT237W V05.26`. A small reference implementation with a
self-test is straightforward to mirror from the BT repo's `ByteEncDecoder` using these tables and the final
`(t3 - pos - k1)` step.

The decoded bytes are the JURA UART command stream (`TY:`, `FA:`, `FN:`, the `@T1/@T2/@T3` handshake…).
This is distinct from the machine-side 4-bytes-per-byte `0x5B` UART encoding (`uart-protocol.md`); the
`*` channel is the WiFi transport's own layer on top.

[Jutta-Proto/protocol-bt-cpp]: https://github.com/Jutta-Proto/protocol-bt-cpp

## Using it for local control
1. Join the dongle's open SoftAP (or put it on your LAN); TCP-connect to `dongle_ip:51515`.
2. Send the **`@HP:` handshake first** (the dispatcher closes the socket otherwise), then `@HY` to
   identify, `@HI`/`@HL` for signature/version. **Verified replies:** `@HP:0,0,0`→`@hp4`,
   `@HY`→`@hy:TT237W V05.26`, `@HI`→`@hi:F02371101103F03F`, `@HL`→`@hl:ESP32LDRSIM TT237W V05.26`,
   `@HU?`→`@hu:800`.
3. Set WiFi with `@HW`; read status with `@HR`; reboot with `@HT3`.
4. Send machine commands over the `*` channel (encode/decode with the shuffle above); OTA the dongle with
   `@HO`/`@HD`/`@HE`.

> **Note on reply encoding:** when no coffee machine is attached the dongle `*`-obfuscates even its `@h…`
> replies; with a machine present (a "session active" state) it returns them as plain ASCII. Either way,
> decode with the `*` shuffle above.

**Still needs a coffee machine to confirm:** the exact `@HP:` field semantics, the `@HR` register meanings
and `@HB` discovery/UDP payload format (both require machine/STA-up state), and the live `*` machine command
stream. A Wireshark + UART capture of the J.O.E. app driving a real machine will nail these down.

[mkalen/jura-smartconnect-wifi]: https://github.com/mkalen/jura-smartconnect-wifi
