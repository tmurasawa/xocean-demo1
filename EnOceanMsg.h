#include <avr/EEPROM.h>

/*
  EnOceanMsg.h - Library for flashing EnOceanMsg code.
 
 Created by David A. Mellis, November 2, 2007.
 Modified by K.Konishi , October 2, 2014
 
 Released into the public domain.
 */


#ifndef EnOceanMsg_h

#define EnOceanMsg_h


#include "Arduino.h"


#define START_BYTE 0x55

class EnOceanMsg
{
public:

  EnOceanMsg();

  void decode();
  
  void initeth();

  void reset();

  bool dataAvailable();

  void prettyPrint();

  uint16_t getPacketLength();

  int getPayload();

  uint32_t getSenderId();

public:

  uint8_t _pos;

  uint8_t _dataLength1;

  uint8_t _dataLength2;

  int _optLength;

  int _packetType;

  int _headerCrc8;

  unsigned char _senderId[4];

  unsigned char _org;

  unsigned char  _payload[4];
  
  unsigned char _rssi;
  

  bool _repeat_flag;
  
  bool _dataReceived;
};


#endif

