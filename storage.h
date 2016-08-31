#pragma once

#include <FS.h>
#include <ESP8266TrueRandom.h>
#include <ArduinoJson.h>
#include "api.h"

#define MEASUREMENT_JSON_SIZE (JSON_OBJECT_SIZE(5))

struct Measurement {
  char timeStamp[20];
  float lat;
  float lng;
  float value;
  char sensorID[32];
};

class Storage {
  protected:
  // not needed, as we post the data as json string?
  /*Measurement deserializeMeasurement(char* s) {
    Measurement m; 
    StaticJsonBuffer<MEASUREMENT_JSON_SIZE> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(s);
    m.timeStamp = (const char[sizeof Measurement.timeStamp])jsonBuffer.strdup(root["date"]);
    m.lat = root["lat"];
    m.lng = root["lng"];
    m.value = root["value"];
    m.sensorID = root["sensorId"];
    return m;
  }*/

  bool serializeMeasurement(Measurement& m, Print& f) {
    f.println(m.sensorID);
    StaticJsonBuffer<MEASUREMENT_JSON_SIZE> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    // TODO: replace temporary data model
    root["createdAt"] = m.timeStamp;
    root["lat"] = m.lat;
    root["lng"] = m.lng;
    root["value"] = m.value;
    root.printTo(f);
    return root.success();
  }
  
  public:
  Storage() {}

  size_t begin() {
    SPIFFS.begin();
    FSInfo fs;
    SPIFFS.info(fs);
    return fs.totalBytes - fs.usedBytes;
  }
  
  bool add(Measurement& m, const char* directory = "/measurements/") {
    byte uuid[16];
    ESP8266TrueRandom.uuid(uuid);
    // we need to shorten the uuid, as long filenames are not supported it seems..?
    String fileName = directory + ESP8266TrueRandom.uuidToString(uuid).substring(26);
    
    if (File f = SPIFFS.open(fileName, "w") ) {
      bool success = serializeMeasurement(m, f);
      f.close();
      return success;
    }
    return false;
  }

  String pop(const char* directory = "/measurements/") {
    Dir dir = SPIFFS.openDir(directory);
    String measurement = "";
    if (!dir.next()) return measurement; // abort if storage is empty
    String fileName = dir.fileName();
    File f = dir.openFile("r");
    measurement = f.readString();
    f.close();
    SPIFFS.remove(fileName);
    return measurement;
  }

  uint16_t size(const char* directory = "/measurements/") {
    Dir dir = SPIFFS.openDir(directory);
    uint16_t i = 0;
    while(dir.next()) i++;
    return i;
  }
  
};
