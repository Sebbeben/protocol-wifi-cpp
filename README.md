# JURA WiFi Protocol
`C++` JURA WiFi protocol implementation for controlling a JURA coffee maker over a WiFi connection.

**Currently still work in progress since I do not own a compatible JURA coffee maker for the WiFi Connect dongle.**

For a device to be able to connect to an JURA coffee maker via WiFi, a [WiFi Connect](https://uk.jura.com/en/homeproducts/accessories/WiFi-Connect-24160) dongle is required.

![WiFi Connect dongle](ressources/images/wifi_dongle_case.png)

## Dumping the Firmware
![ESP32 inside the WiFi Connect dongle](ressources/images/wifi_dongle_esp32.png)

I use this command to dump the entire flash memory of my Jura WiFi Device

.\esptool.exe --baud 115200 --port COM6 read_flash 0x0 0x400000 jura-backup-4M.bin
[Dumping Firmware](ressources/images/Firmwaredump_sucessfull.png)
