# JURA WiFi Protocol
`C++` JURA WiFi protocol implementation for controlling a JURA coffee maker over a WiFi connection.

**Currently still work in progress since I do not own a compatible JURA coffee maker for the WiFi Connect dongle.**

For a device to be able to connect to an JURA coffee maker via WiFi, a [WiFi Connect](https://uk.jura.com/en/homeproducts/accessories/WiFi-Connect-24160) dongle is required.

![WiFi Connect dongle](ressources/images/wifi_dongle_case.png)

## Dumping the Firmware
i soldered wired to the pads and connected 3.3v, gnd TX & RX
![ESP32 inside the WiFi Connect dongle](ressources/images/jura_wiring.png)

I use this command to dump the entire flash memory of my Jura WiFi Device

a tricky thing is to get the ESP into programming mode, this took me a very long time to do, you have to short GND and GPIO 1 when powering up.
.\esptool.exe --baud 115200 --port COM6 read_flash 0x0 0x400000 jura-backup-4M.bin
![Dumping Firmware](ressources/images/Firmwaredump_sucessfull.png)

---------------------------------------------------------

3D Printed Housing

3D printed case/images/JURA Smart Connect WiFi top.png
3D printed case/images/JURA Smart Connect WiFi bottom.png
3D printed case/images/compare1.jpg
3D printed case/images/compare2.jpg
3D printed case/images/side1.jpg
3D printed case/images/side2.jpg
3D printed case/images/side3.jpg
Sebbeben/protocol-wifi-cpp/3D printed case/images/top1.jpg

---------------------------------------------------------

PCB Reverse Engineered

PCB Files/images/bottom_ex_pinheader.jpg
PCB Files/images/top_ex_esp32.jpg