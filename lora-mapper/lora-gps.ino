/**
   lora-gps senseBox with SDS011 particulate matter + HDC1008 temp & humi sensors
   for Arduino Mega with Dragino LoRa shield.
   SDS is on SDS_SERIAL, GPS on GPS_SERIAL
*/

#include "config.h"
#include "lora.h"
#include <SoftwareSerial.h>

#include <LoraMessage.h>

//#include <avr/wdt.h>

#include <TinyGPS++.h>
TinyGPSPlus gps;

SoftwareSerial ss(3, 4);

void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    LoraMessage message;

    //-----GPS-----//
    bool newData = false;
    float lat, lng;
    for (unsigned long start = millis(); millis() - start < 5000;) // timeout
    {
      // TODO
      //while (gps.location.isUpdated() && gps.location.isValid())
      while (ss.available())
      {
        char c = ss.read();
        if (gps.encode(c)) // Did a new valid sentence come in?
          newData = true;
      }
    }
    if (newData) {
      lat = gps.location.lat();
      lng = gps.location.lng();
      float alt = gps.altitude.meters();
      float hdop = gps.hdop.hdop();
      Serial.print("latitude: ");
      Serial.println(lat, 6);
      Serial.print("longitude: ");
      Serial.println(lng, 6);
      Serial.print("altitude: ");
      Serial.println(alt, 6);
      Serial.print("hdop: ");
      Serial.println(hdop, 6);
      message.addLatLng(lat, lng);
      message.addTemperature(alt);
      message.addTemperature(hdop);
    } else {
      return;
    }

    delay(300);

    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(1, message.getBytes(), message.getLength(), 0);
    Serial.println(F("LoRa Packet queued"));

  }
  // Next TX is scheduled after TX_COMPLETE event.
}

void initSensors() {
  //Initialize sensors
  Serial.print("Initializing sensors...");

  // initialize GPS Serial Port
  ss.begin(9600);
  while (!ss.available()) {
    Serial.println("detecting GPS device...");
    delay(1000);
  }
  Serial.println("Wait for GPS...");
  while (!gps.location.isValid()) {
    gps.encode(ss.read());
    delay(1);
  }
  Serial.println("Got GPS fix!");
}

void setup() {
  Serial.begin(9600); // debug serial
  Serial.println(F("Starting"));

#ifdef VCC_ENABLE
  // For Pinoccio Scout boards
  pinMode(VCC_ENABLE, OUTPUT);
  digitalWrite(VCC_ENABLE, HIGH);
  delay(1000);
#endif

  initSensors();

  Serial.println("initializing LoRa..");

  // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    // Set static session parameters. Instead of dynamically establishing a session
    // by joining the network, precomputed session parameters are be provided.
    #ifdef PROGMEM
    // On AVR, these values are stored in flash and only copied to RAM
    // once. Copy them to a temporary buffer here, LMIC_setSession will
    // copy them into a buffer of its own again.
    uint8_t appskey[sizeof(APPSKEY)];
    uint8_t nwkskey[sizeof(NWKSKEY)];
    memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
    memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
    LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);
    #else
    // If not running an AVR with PROGMEM, just use the arrays directly
    LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);
    #endif

    #if defined(CFG_eu868)
    // Set up the channels used by the Things Network, which corresponds
    // to the defaults of most gateways. Without this, only three base
    // channels from the LoRaWAN specification are used, which certainly
    // works, so it is good for debugging, but can overload those
    // frequencies, so be sure to configure the full frequency range of
    // your network here (unless your network autoconfigures them).
    // Setting up channels should happen after LMIC_setSession, as that
    // configures the minimal channel set.
    // NA-US channels 0-71 are configured automatically
    LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
    LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
    // TTN defines an additional channel at 869.525Mhz using SF9 for class B
    // devices' ping slots. LMIC does not have an easy way to define set this
    // frequency and support for class B is spotty and untested, so this
    // frequency is not configured here.
    #elif defined(CFG_us915)
    // NA-US channels 0-71 are configured automatically
    // but only one group of 8 should (a subband) should be active
    // TTN recommends the second sub band, 1 in a zero based count.
    // https://github.com/TheThingsNetwork/gateway-conf/blob/master/US-global_conf.json
    LMIC_selectSubBand(1);
    #endif

    // Disable link check validation
    LMIC_setLinkCheckMode(0);

    // TTN uses SF9 for its RX2 window.
    LMIC.dn2Dr = DR_SF9;

    // Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
    LMIC_setDrTxpow(DR_SF7,14);

    // Start job
    do_send(&sendjob);
}

void loop() {
  os_runloop_once();
}
