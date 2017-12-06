# wifi-gps senseBox

This is a GPS tracked senseBox measuring particulate matter concentrations, transmitting its data via WiFi to [openSenseMap].

[openSenseMap]: opensensemap.org

It's based on
- Arduino Mega (Arduino Uno should work to, just use SoftwareSerial for the SDS011)
- Dragino LoRa- & GPS-Shield (only used to acquire GPS data)
- BlueFly WiFi Shield (ATWINC1500)
- Novafit SDS011 particulate matter sensor
- all standard senseBox sensors (HDC1008, BMP280, VEML6070, TSL45315)

For mobile power supply I used an Adafruit LiPo charger + 6600mAh LiPo.

## Notes about device operation
The device only measures and transmits once there was an initial GPS fix and a successful WiFi connection. This means the device only begins operating when placed outdoors.

## Sketch setup & opsensensemap integration

- Register a senseBox at <https://opensensemap.org/register>
  - select `mobile` for exposure
  - select the location where you plan the first deployment
  - select the model `senseBox:home` with `fine-dust` extension
- Insert the SSID & network password in `wifi-gps.ino` at line 22-23

## Hardware Setup
- Flash the sketch using Arduino IDE
- Stack the senseBox Shield onto the Arduino, then fist stack the BlueFly WiFi shield and then Dragino shield on top.
- Remove the `GPS_TX` and `GPS_RX` jumpers on the Dragino Shield, and wire these pins instead to the Arduinos `RX3` and `TX3` pins.
- Wire the SDS011: RX & TX go to `RX2` and `TX2` on the Arduino, GND to GND, 5V to 5V ;)
- Connect the LiPo to the LiPo-Charger, and connect the USB Output to the Arduino
- Done!
