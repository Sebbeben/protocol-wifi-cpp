# protocol/

Reverse-engineered protocols of the JURA WiFi Connect (TT237W) dongle.

> **⚠️ Reverse-engineered from firmware via static analysis — NOT yet verified on live hardware.**
> Command names, ports, byte layouts and the codecs below are a working hypothesis recovered by
> decompiling the dongle firmware and the J.O.E. app. They have **not** been confirmed with a real
> device/machine yet. Verify against a packet/UART capture before relying on them; corrections welcome.

| Document | What |
|----------|------|
| [`firmware-images.md`](firmware-images.md) | **Read first.** Which of the two firmware images actually boots (a custom JURA LAN app) vs. which is dormant (stock ESP-AT). |
| [`wifi-local-protocol.md`](wifi-local-protocol.md) | **The headline.** The dongle's LAN control protocol — TCP/UDP **port 51515**, the `@H…` command set, and the `*`-prefixed encoded coffee-machine passthrough. This is how you control the stock dongle locally with no cloud. |
| [`uart-protocol.md`](uart-protocol.md) | The JURA machine UART protocol the dongle speaks downstream: 9600 8N1, the keyless **0x5B** transfer encoding, V2 handshake, and the older **V1** command vocabulary. |
| [`cloud-architecture.md`](cloud-architecture.md) | How JURA's cloud is wired (Keycloak / `joeapi.jura.com` / *pocketpilot*) and how the phone app reaches the dongle — context for self-hosting. |

## The big picture
```
   Coffee machine  <--UART 9600 8N1, 0x5B codec-->  [ WiFi Connect dongle (ESP32) ]
                                                            |
                                       LAN: TCP/UDP 51515, @H… + * channel   <-- local control (no cloud)
                                                            |
                                       (running FW = custom JURA app, LAN-only, no cloud client)
```
The dongle is fundamentally a **bridge**: machine-UART on one side, WiFi on the other. The most
practical local-control path is its **LAN protocol** (`wifi-local-protocol.md`).
