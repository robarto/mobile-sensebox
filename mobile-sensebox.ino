#include "config.h"
#include "gps.h"
#include "wifi.h"
#include "api.h"
#include "storage.h"
#include "TelnetPrint.h"

#define DEBUG_OUT Serial

//TelnetPrint telnet = TelnetPrint();
Storage storage = Storage();

/* UTILS */
void printState(WifiState wifi) {
  DEBUG_OUT.print("homeAvailable: ");
  DEBUG_OUT.println(wifi.homeAvailable);
  DEBUG_OUT.print("numNetworks: ");
  DEBUG_OUT.println(wifi.numNetworks);
  DEBUG_OUT.print("numUnencrypted: ");
  DEBUG_OUT.println(wifi.numUnencrypted);

  DEBUG_OUT.print("lat: ");
  //DEBUG_OUT.print(gps.location.lat(), 6);
  DEBUG_OUT.print(" lng: ");
  //DEBUG_OUT.println(gps.location.lng(), 6);

  DEBUG_OUT.println(getISODate());
  DEBUG_OUT.println("");
}


/* MAIN ENTRY POINTS */
void setup() {
  //Serial.begin(9600); // GPS reciever
  DEBUG_OUT.begin(115200);

  WiFi.mode(WIFI_STA);
  //connectWifi(WIFI_SSID, WIFI_PASS);

  //delay(5000); // DEBUG: oportunity to connect to network logger

  // wait until we got a first fix from GPS, and thus an initial time
  /*DEBUG_OUT.print("Getting GPS fix..");
  while (!updateLocation()) { DEBUG_OUT.print("."); }
  DEBUG_OUT.print(" done! ");*/
  DEBUG_OUT.println(getISODate());

  DEBUG_OUT.println("Setup done!\n");
  DEBUG_OUT.println("WiFi MAC            WiFi IP");
  DEBUG_OUT.print(WiFi.macAddress());
  DEBUG_OUT.print("   ");
  DEBUG_OUT.println(WiFi.localIP());
  
  DEBUG_OUT.print("SPIFF bytes free: ");
  DEBUG_OUT.println(storage.begin());
  
  digitalWrite(D9, HIGH); // DEBUG: integrated  led? doesnt work
}

void loop() {
  //pollGPS();
  //DEBUG_OUT.pollClients();
  WifiState wifi = scanWifi(WIFI_SSID);

  // TODO: take other measurements (average them?)
/*
  if(!updateLocation()) DEBUG_OUT.println("GPS timed out (location)");
  if(!updateTime()) DEBUG_OUT.println("GPS timed out (time)");
*/
  // TODO: write measurements to file

  // TODO: connect to wifi, if available & not connected yet
    // then upload local data, clean up

  printState(wifi);

  // recall all previous measurements
  while (storage.size()) {
    String measure = storage.pop();
    DEBUG_OUT.println("popped a measurement: ");
    DEBUG_OUT.println(measure.substring(0, 31)); // size of sensorID
    DEBUG_OUT.println(measure.substring(32));    // skip the newline char
  }

  // store new measurement
  Measurement testMeasure;
  testMeasure.lat = 51.2;
  testMeasure.lng = 7.89;
  testMeasure.value = wifi.numNetworks;
  strcpy(testMeasure.timeStamp, getISODate());
  strcpy(testMeasure.sensorID, "123457812345678123456781234567");
  
  if (storage.add(testMeasure)) {
    DEBUG_OUT.print("measurement stored! storage size: ");
  } else {
    DEBUG_OUT.print("measurement store failed! storage size: ");
  }
  DEBUG_OUT.println(storage.size());
  
  delay(2000);
}