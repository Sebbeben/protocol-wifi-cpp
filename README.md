# JURA WiFi Protocol
`C++` JURA WiFi protocol implementation for controlling a JURA coffee maker over a WiFi connection.

**Currently still work in progress since I do not own a compatible JURA coffee maker for the WiFi Connect dongle.**

For a device to be able to connect to an JURA coffee maker via WiFi, a [WiFi Connect](https://uk.jura.com/en/homeproducts/accessories/WiFi-Connect-24160) dongle is required.

![WiFi Connect dongle](ressources/images/wifi_dongle_case.png)

## Dumping the Firmware
i soldered wired to the pads and connected 3.3v, GND, TX, RX & GPIO 0
![ESP32 inside the WiFi Connect dongle](ressources/images/jura_wiring.png)

I use this command to dump the entire flash memory of my Jura WiFi Device

a tricky thing is to get the ESP into programming mode, this took me a very long time to do, you have to short GND and GPIO 1 when powering up.

then i used this command after installing esptool for windows.


        .\esptool.exe --baud 115200 --port COM6 read_flash 0x0 0x400000 jura-backup-4M.bin
![Dumping Firmware](ressources/images/Firmwaredump_sucessfull.png)

---------------------------------------------------------

3D Printed Housing

![3D_Model_Housing](3D_printed_case/images/JURA_Smart_Connect_WiFi_top.png) width="800"
![3D_Model_Housing](3D_printed_case/images/JURA_Smart_Connect_WiFi_bottom.png)
![3D_Model_Housing](3D_printed_case/images/compare1.jpg)
![3D_Model_Housing](3D_printed_case/images/compare2.jpg)
![3D_Model_Housing](3D_printed_case/images/side1.jpg)
![3D_Model_Housing](3D_printed_case/images/side2.jpg)
![3D_Model_Housing](3D_printed_case/images/side3.jpg)
![3D_Model_Housing](3D_printed_case/images/top1.jpg)

---------------------------------------------------------
