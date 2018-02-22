#include <lmic.h> // for the key data type u1_t

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 5;

/* LoRaWAN NwkSKey, network session key
   This is the default Semtech key, which is used by the prototype TTN
   network initially.
   ttn*/
static const PROGMEM u1_t NWKSKEY[16] =  { 0x6E, 0x79, 0x93, 0xB3, 0x55, 0xF2, 0x1A, 0x74, 0x2C, 0xA2, 0x5A, 0x6B, 0xFC, 0xFA, 0xBD, 0xAE };
/* LoRaWAN AppSKey, application session key
   This is the default Semtech key, which is used by the prototype TTN
   network initially.
   ttn*/
static const u1_t PROGMEM APPSKEY[16] = { 0xAB, 0x0C, 0x79, 0xF3, 0xFA, 0xA2, 0x6C, 0x16, 0x8A, 0x24, 0xE0, 0x2C, 0x62, 0x87, 0x2B, 0xC3 };

/*
 LoRaWAN end-device address (DevAddr)
 See http://thethingsnetwork.org/wiki/AddressSpace
 ttn*/
static const u4_t DEVADDR = 0x2601134D;
