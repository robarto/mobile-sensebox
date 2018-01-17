#include <SysCall.h>
#include <FreeStack.h>
#include <SdFatConfig.h>
#include <SdFat.h>
#include <BlockDriver.h>
#include <MinimumSerial.h>

#include <TinyGPS++.h>

SdFat SD;
File logFile;

bool writeToSD (float temperature, float humidity, float p25, float p10, TinyGPSPlus gps, int co2, float co2temperature) {
  logFile = SD.open("datalog.csv", FILE_WRITE);
  if (!logFile) return false;

  logFile.print(temperature, 2);
  logFile.print(",");
  logFile.print(humidity, 2);
  logFile.print(",");
  logFile.print(p25, 2);
  logFile.print(",");
  logFile.print(p10, 2);
  logFile.print(",");
  logFile.print(co2);
  logFile.print(",");
  logFile.print(co2temperature, 2);
  logFile.print(",");
  logFile.print(gps.location.lat(), 6);
  logFile.print(",");
  logFile.print(gps.location.lng(), 6);
  logFile.print(",");

  logFile.print(gps.date.year());
  logFile.print(F("-"));
  if (gps.date.month() < 10) logFile.print(F("0"));
  logFile.print(gps.date.month());
  logFile.print(F("-"));
  if (gps.date.day() < 10) logFile.print(F("0"));
  logFile.print(gps.date.day());
  logFile.print(F("T"));
  logFile.print(gps.time.hour());
  logFile.print(F(":"));
  if (gps.time.minute() < 10) logFile.print(F("0"));
  logFile.print(gps.time.minute());
  logFile.print(F(":"));
  if (gps.time.second() < 10) logFile.print(F("0"));
  logFile.print(gps.time.second());
  logFile.println(F("Z"));
  
  logFile.close();
}

