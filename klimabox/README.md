# Klimabox

This is a GPS tracked senseBox measuring particulate matter and CO2 concentrations developed for a study project. It saves all its measurements on a SD card in csv format.

It's based on
- Arduino Mega (Arduino Uno should work to, just use SoftwareSerial for the SDS011)
- Dragino LoRa- & GPS-Shield
- SD-card reader (I used the one on the senseBox-Shield. Any other one should work as well, change the `SD_PIN` in `config.h`)
- Novafit SDS011 particulate matter sensor
- HDC1008 temp- & humidity sensor
- Adafruit Powerboost 1000C incl. 6600mAh Battery
- MH-Z16 CO2 sensor

## Notes about device operation
The device only measures and transmits once there was an initial GPS fix.

## Hardware Setup
- Flash the sketch using Arduino IDE
- Stack the senseBox Shield onto the Arduino, then stack the Dragino shield on top.
- Remove the `GPS_TX` and `GPS_RX` jumpers on the Dragino Shield, and wire these
  pins instead to the Arduinos `RX3` and `TX3` pins.
- Wire the SDS011: RX & TX go to `RX2` and `TX2` on the Arduino, GND to GND, 5V to 5V ;)
- Wire the MH-Z16 CO2 sensor: RX & TX go to `RX1` and `TX1` on the Arduino, GND to GND, 5V to 5V ;)
- Connect the LiPo to the LiPo-Charger, and connect the USB Output to the Arduino
- Done!
