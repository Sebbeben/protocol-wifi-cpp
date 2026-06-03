# JURA cloud architecture (context for self-hosting)

How the official stack is wired, recovered from the **J.O.E. Android app** (`ch.toptronic.joe`) and the
dongle firmware. Useful if you want to self-host a replacement server — though the simpler local route is
the dongle's own LAN protocol (`wifi-local-protocol.md`).

> **⚠️ Observed from the app/firmware; endpoints may change. Not exhaustive.**

## Endpoints (from the J.O.E. app)
| Purpose | Endpoint |
|---------|----------|
| Auth (OIDC) | `https://keycloak.pocketpilot.jura.com/realms/Jura/protocol/openid-connect/{auth,token,…}` |
| Cloud API | `https://joeapi.jura.com/api/` |
| Machine list / assets | `https://digitalassets.jura.com/mobileapp/JOE/JOE_MACHINES.TXT` |
| WiFi onboarding | `jura.com/wifi` |

- Auth is **Keycloak / OpenID Connect**, realm **`Jura`**; JURA's IoT backend is codenamed **pocketpilot**.
- The modern app is **REST-based** (no XMPP client in the app). The dongle firmware contains an XMPP/TLS
  client and an ESP-AT OTA path; the cloud bridges the app's REST calls to the dongle.

## What this means
- The phone app does **not** talk to the dongle directly for cloud control — it goes app → cloud → dongle.
- But the dongle **also** runs a local LAN server (see `wifi-local-protocol.md`), so for local control you
  can bypass the cloud entirely.
- The dongle's cloud credentials (JID/token) are **provisioned at pairing** and are not present in a
  factory-fresh firmware dump — capturing them (or the LAN protocol) requires a live device.

## Important: these cloud/OTA strings are in a DORMANT firmware image
A stock **ESP-AT** firmware image is present in the dongle's flash (with an XMPP client and a
`download_rom`/`+CIPUPDATE` OTA path pointing at Espressif's demo host `iot.espressif.cn`, using
Espressif's **example** TLS CA with **no certificate verification**). **But that image is not booted** —
the running firmware is a custom JURA app with **no cloud client** (see
[`firmware-images.md`](firmware-images.md)). So those cloud/OTA/TLS details describe the **dormant**
image, **not** the live dongle. Earlier notes calling the dongle "ESP-AT based" were reading that
dormant image.
