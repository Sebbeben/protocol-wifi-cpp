# JURA machine UART protocol (downstream side)

The dongle talks to the coffee machine over the machine's service-port UART. This protocol was
documented by prior projects — most of this section is **their** work, summarised here for context;
see credits. The running firmware contains a UART driver plus the `TY:`/`@T1` handshake and the 0x5B
codec (seen in decompilation); higher-level product commands appear to be relayed from the WiFi
`*`-channel rather than hard-coded. Treat the dongle-specific specifics as unverified.

> **⚠️ The dongle-specific details are reverse-engineered and unverified;** the general JURA UART
> protocol below is well-established by the cited projects.

## Line settings
- **9600 baud, 8 data bits, no parity, 1 stop bit**, no flow control, 5 V TTL.
- Commands are uppercase ASCII terminated `\r\n`; responses are lowercase; most commands ack with `ok:`.

## The "0x5B" transfer encoding (keyless)
Every data byte is sent as **4 raw UART bytes** (≈8 ms apart). Each raw byte uses base pattern
`0b01011011 = 0x5B` and carries only 2 data bits (at bit positions 2 and 5). It is a deterministic
bit-shuffle, **not** a cipher (no key). Decode reads bytes in groups of 4; partial groups are discarded.
The exact encode/decode is in [Jutta-Proto/protocol-cpp] (`JuttaConnection.cpp`) and
[COM8/esp32-jura]; a worked example and captures are in those repos.

```
encoded byte:  0 1 0 1 1 0 1 1   (0x5B)
                   ^     ^
                  bit2  bit5  <- the only data-bearing positions
```

## Identification & handshake
- `TY:` → `ty:EF###M Vxx.yy` — the machine **type** (the `EF###` is the machine-file key) + firmware version.
- Newer (**V2**) machines do a key-exchange before some traffic:
  `@T1 / @t1 / @T2:… / @t2:… / @T3:…<type> / @t3` (the `@T3` echoes the machine type).

## Command vocabulary
- **V2 (Jutta-Proto/protocol-cpp):** `AN:` power/mode, `FA:04..09` UI buttons 1–6, `FN:` actuators
  (pump/heater/grinder/brew-group), product brewing via page+button emulation or a manual actuator
  sequence. Full table in the cited repos.
- **V1 (older machines, [juramote]):** richer debug vocabulary — `HZ:` status (temps, flow, brew sensor),
  `IC:` input board, `RE:`/`RT:` read EEPROM, `WE:` write EEPROM, `DA:`/`DT:`/`DR:` display control,
  `TY:` type, `GB:` off. Useful for the "older machines only on Bluetooth today" goal.

## Per-machine products
The J.O.E. app ships per-machine XML (`EF###.xml`) describing each product and its command arguments
(`F3`=strength, `F4`=water amount, `F7`=temperature, with min/max/step). These map an `EF###` (from
`TY:`) to the exact brew parameters — the basis for supporting arbitrary machines.

## Credits
This protocol is documented by [Jutta-Proto/protocol-cpp], [Jutta-Proto/protocol-bt-cpp],
[COM8/esp32-jura], and [juramote]. See [`../REFERENCES.md`](../REFERENCES.md).

[Jutta-Proto/protocol-cpp]: https://github.com/Jutta-Proto/protocol-cpp
[Jutta-Proto/protocol-bt-cpp]: https://github.com/Jutta-Proto/protocol-bt-cpp
[COM8/esp32-jura]: https://github.com/COM8/esp32-jura
[juramote]: https://6xq.net/juramote/
