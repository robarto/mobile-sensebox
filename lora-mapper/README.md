# lora-gps senseBox

This is a GPS tracked TTN coverage mapper,
transmitting its data via LoRaWAN through [TheThingsNetwork] to [TTN Mapper].

[TheThingsNetwork]: thethingsnetwork.org
[TTN Mapper]: http://ttnmapper.org/

It's based on
- Arduino UNO
- Dragino LoRa- & GPS-Shield

For mobile power supply I used an Adafruit LiPo charger + 5.6Ah LiPo.

## Notes about device operation
The device only measures and transmits once there was an initial GPS fix. This means the device only begins operating when placed outdoors.

## Sketch setup & opsensensemap integration

- Register a device under <https://console.thethingsnetwork.org>
  - register a new application first
  - register a new device
  - visit the settings of your device
  - in the General section, enable the `ABP` Activation Method and disable `Frame Counter Checks`
  - note down the following values: Device Address, Network Session Key and App Session Key
  - in the Application, go to `Payload Formats` and insert the Decoder from [https://gist.github.com/felixerdy/362d4f4ee6b13c99f4635e7d6aec4d5f](https://gist.github.com/felixerdy/362d4f4ee6b13c99f4635e7d6aec4d5f) (copy and paste) and save the decoder
  - on Integration, add the TTN Mapper integration and insert your email address
- Insert the Device Address, Network Session Key and App Session Key you received from TTN in `config.h`

## Hardware Setup
- Flash the sketch using Arduino IDE
- Stack the Dragino shield onto the Arduino
- Remove the `GPS_TX` and `GPS_RX` jumpers on the Dragino Shield, and wire these pins instead to the Arduinos `3` and `4` pins.
- Connect the LiPo to the LiPo-Charger, and connect the USB Output to the Arduino
- Done!
