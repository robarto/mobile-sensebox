#include "sd.h"

#include <HDC100X.h>
#include <SDS011-select-serial.h>

//#include <avr/wdt.h>

#include <TinyGPS++.h>
TinyGPSPlus gps;

#define SD_PIN 4
#define SDS_SERIAL Serial3
#define GPS_SERIAL Serial2
#define CO2_SERIAL Serial1

//Load sensors
SDS011 my_sds(SDS_SERIAL);
HDC100X HDC(0x43);

//measurement variables
float temperature = 0, humidity = 0, p10 = 0, p25 = 0;
int error;

// co2
const unsigned char cmd_get_sensor[] =
{
  0xff, 0x01, 0x86, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x79
};
unsigned char dataRevice[9];
int co2temperature;
int CO2PPM;
float lat, lng;

void initSensors() {
  //Initialize sensors
  Serial.print("Initializing sensors...");
  Wire.begin();
  HDC.begin(HDC100X_TEMP_HUMI, HDC100X_14BIT, HDC100X_14BIT, DISABLE);
  CO2_SERIAL.begin(9600);
  Serial.println("done!");
  temperature = HDC.getTemp();

  // initialize GPS Serial Port
  GPS_SERIAL.begin(9600);
  while (!GPS_SERIAL.available()) {
    Serial.println("detecting GPS device...");
    delay(1000);
  }
  Serial.println("Wait for GPS...");
  while (!gps.location.isValid()) {
    gps.encode(GPS_SERIAL.read());
    delay(1);
  }
  Serial.println("Got GPS fix!");

  // init SD card
  Serial.print("Initializing SD card...");

  if (!SD.begin(SD_PIN)) {
    Serial.println("failed!");
    return;
  }
  Serial.println("done!");

  // initalize SDS Serial Port

  SDS_SERIAL.begin(9600);
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

  // sd card
  pinMode(4, INPUT);
  digitalWrite(4, HIGH);

  initSensors();
}

void loop() {
  //-----GPS-----//
  bool newData = false;

  for (unsigned long start = millis(); millis() - start < 5000;) // timeout
  {
    // TODO
    //while (gps.location.isUpdated() && gps.location.isValid())
    while (GPS_SERIAL.available())
    {
      char c = GPS_SERIAL.read();
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }
  if (newData) {
    lat = gps.location.lat();
    lng = gps.location.lng();
    Serial.print("latitude: ");
    Serial.println(lat, 6);
    Serial.print("longitude: ");
    Serial.println(lng, 6);
  } else {
    return;
  }

  //-----Temperature-----//
  Serial.print("temperature: ");
  temperature = HDC.getTemp();
  Serial.println(temperature);
  //-----Humidity-----//
  Serial.print("humidity: ");
  humidity = HDC.getHumi();
  Serial.println(humidity);

  //-----fine dust-----/
  error = my_sds.read(&p25, &p10);
  if (!error) {
    Serial.println("P2.5: " + String(p25));
    Serial.println("P10:  " + String(p10));
  }

  //-----co2------/
  if (dataRecieve())
  {
    Serial.print("Temperature: ");
    Serial.print(co2temperature);
    Serial.print("  CO2: ");
    Serial.print(CO2PPM);
    Serial.println("");
  }

  Serial.println(F("Writing to SD card."));
  writeToSD(temperature, humidity, p25, p10, gps, CO2PPM, co2temperature);
}

bool dataRecieve(void)
{
  byte data[9];
  int i = 0;
  //transmit command data
  for (i = 0; i < sizeof(cmd_get_sensor); i++)
  {
    CO2_SERIAL.write(cmd_get_sensor[i]);
  }
  delay(10);
  //begin reveiceing data
  if (CO2_SERIAL.available())
  {
    while (CO2_SERIAL.available())
    {
      for (int i = 0; i < 9; i++)
      {
        data[i] = CO2_SERIAL.read();
      }
    }
  }
  for (int j = 0; j < 9; j++)
  {
    Serial.print(data[j]);
    Serial.print(" ");
  }
  Serial.println("");
  if ((i != 9) || (1 + (0xFF ^ (byte)(data[1] + data[2] + data[3] + data[4] + data[5] + data[6] + data[7]))) != data[8])
  {
    return false;
  }
  CO2PPM = (int)data[2] * 256 + (int)data[3];
  co2temperature = (int)data[4] - 40;
  return true;
}
