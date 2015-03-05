/*
  EnOceanMsg.cpp - Library for flashing EnOceanMsg code.
 Created by David A. Mellis, November 2, 2007.
  ver0.01 -Modified by K.Konishi , October 2 , 2014
  ver0.02 -Fixed output of serial monitor by K.Konishi , December 4, 2014
 Released into the public domain.
 */

#include "Arduino.h"
#include "EnOceanMsg.h"
#include <PubSubClient.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)

// dtostrf buffer
char s[16];
char t[16];

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 9);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient Ethclient;


// No callback
void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}


// scalenics
#define DEVICE_TOKEN "YOUR_DEVICE_TOKEN_HERE"
// #define DEVICE_ID "arduino01"
#define CLIENT_ID "enocean"
#define MQTT_SERVER "api.scalenics.io"
// MQTT client
PubSubClient client2(MQTT_SERVER, 1883, callback, Ethclient);

String topic;
String PostData;
char mqtt_topic[128];
char mqtt_payload[64];


EnOceanMsg::EnOceanMsg()
{
  reset();
}

void EnOceanMsg::initeth()
{
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    Ethernet.begin(mac, ip);
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();
}

void EnOceanMsg::reset()
{
  _dataLength1 = 0;
  _dataLength2 = 0;
  _optLength = 0;
  _packetType = 0;
  _headerCrc8 = 0;
  _org = 0;
  _payload[0] = 0;
  _payload[1] = 0;
  _payload[2] = 0;
  _payload[3] = 0;


  _senderId[0] = 0;
  _senderId[1] = 0;
  _senderId[2] = 0;
  _senderId[3] = 0;
  _rssi = 0 ;
  _dataReceived = false;
  _repeat_flag = 0 ;


}

uint16_t EnOceanMsg::getPacketLength() {
  //_dataLength1=0x45;
  //_dataLength2=0xA1;
  return ((uint16_t(_dataLength1) << 8) & 0xffff) + (_dataLength2 & 0xff);
}

int EnOceanMsg::getPayload() {
  return _payload[0];
}

bool EnOceanMsg::dataAvailable()
{
  return _dataReceived;
}

uint32_t EnOceanMsg::getSenderId() {
  //_senderId1=0x45;
  //_senderId2=0x46;
  //_senderId3=0x47;
  //_senderId4=0x48;
  uint32_t aResponse = ((uint32_t(_senderId[0]) << 24) & 0xffffffff) + ((uint32_t(_senderId[1])  << 16) & 0xffffffff) + ((uint32_t(_senderId[2]) << 8) & 0xffffffff) + (uint32_t(_senderId[3]) & 0xffffffff);
  return aResponse;
}

void EnOceanMsg::prettyPrint()
{

  Serial.println("Pretty print start");

  char buf1[9];
  char buf2[9];
  char buf3[9];
  char buf4[9];
  char buf[4];

  Serial.print("length:");

  sprintf(buf1, "%04X", getPacketLength());
  Serial.println(buf1);
  //Serial.println(getPacketLength());

  Serial.print("Optional length:");
  sprintf(buf, "%02X", _optLength);

  Serial.println(buf);

  Serial.print("Packet type:0x");
  Serial.println(_packetType, HEX);

  Serial.print("Header:0x");
  Serial.println(_org, HEX);

  switch ((_org & 0x0F)) {
    case (0): /* RPS Telegram */
    case (1): /* 1BS Telegram */


      Serial.print("Payload1:0x");
      sprintf(buf, "%02X", _payload[0]);
      Serial.println(buf);
      break;
    case (2): /* 4BS Telegram */
      Serial.print("Payload1:0x");
      sprintf(buf, "%02X", _payload[0]);
      Serial.println(buf);
      Serial.print("Payload2:0x");

      sprintf(buf, "%02X", _payload[1]);
      Serial.println(buf);
      Serial.print("Payload3:0x");
      sprintf(buf, "%02X", _payload[2]);

      Serial.println(buf);
      Serial.print("Payload4:0x");
      sprintf(buf, "%02X", _payload[3]);

      Serial.println(buf);
      break;
  }

  Serial.print("Sender Id:");

  //sprintf(buf, "%lx", getSenderId());
  //Serial.println(buf);
  sprintf(buf, "%08lX", getSenderId());
  Serial.println(buf);

  Serial.print("RSSI :-");
  Serial.print(_rssi , DEC);
  Serial.println("dbm");

  // make topic (DEVICE_TOKEN/DEVICE_ID)
  sprintf(buf, "%08lX", getSenderId());
  topic = DEVICE_TOKEN;
  topic += "/";
  topic += buf;

  sprintf(buf1, "%02X", _payload[0]);
  String v1 = String(_payload[0], DEC);
  sprintf(buf2, "%02X", _payload[1]);
  String v2 = String(_payload[1], DEC);
  sprintf(buf3, "%02X", _payload[2]);
  String v3 = String(_payload[2], DEC);
  sprintf(buf4, "%02X", _payload[3]);
  String v4 = String(_rssi, DEC);
  PostData = "";
  PostData += "v=";
  PostData += v1;
  PostData += "&v2=";
  PostData += v2;
  PostData += "&v3=";
  PostData += v3;
  PostData += "&v4=";
  PostData += v4;
  //PostData += "&v2=";
  //PostData += dtostrf(h, 4, 1, s);

  topic.toCharArray(mqtt_topic, topic.length() + 1);
  PostData.toCharArray(mqtt_payload, PostData.length() + 1);

  Serial.println("mqtt_topic=");
  Serial.println(mqtt_topic);
  Serial.println("mqtt_payload=");
  Serial.println(mqtt_payload);

  Serial.println("Pretty print end");

  if (client2.connect(CLIENT_ID))
  {
    Serial.println("Connect to MQTT server..");
    client2.publish(mqtt_topic, mqtt_payload);
    Serial.println("Disconnecting MQTT server..");
    client2.disconnect();
  }





}

void EnOceanMsg::decode()
{
  _pos = 0 ;
  //Serial.println("Entering");
  while (Serial.available() > 0)
  {

    uint8_t aChar = Serial.read();
    switch (_pos)
    {
      case 0:        //  Start byte
        if (aChar == START_BYTE)
        {
          delay(5);                /* wait for ESP3 all bytes receive  */
          _pos++;
        }
        break;

      case 1:        //  data length
        _dataLength1 = aChar;
        _pos++;
        break;

      case 2:       //  data length
        _dataLength2 = aChar;
        _pos++;
        break;

      case 3:      //  option data length
        _optLength = aChar;
        _pos++;
        break;

      case 4:      //  packet type
        _packetType = aChar;
        _pos++;
        break;

      case 5:      //  crc(header)
        _headerCrc8 = aChar;
        _pos++;
        break;

      case 6:      //  data header
        _org = aChar;
        if ((_org >> 4) & 0x01) { // extend header exit
          _repeat_flag = 1 ;
        }
        else {                 // no extend header
          _repeat_flag = 0 ;
          _pos++ ;
        }

        switch ((_org >> 5) & 0x07) // Sender ID length check
        {
          case (0):                 // ID 24bit
            _pos++;
            _pos++;
            _pos++;
            break;
          case (1):                // ID 32bit
          case (2):
            _pos++;
            _pos++;
            break;
          case (3):                // ID 48bit
          default:
            break;

        }
        _pos++;
        break;


      case 7:                      // extend header
        _pos++;
        break;

      case 8:                      // if ID 48bit
        _pos++;
        break;

      case 9:                      //  if ID 48bit
        _pos++;
        break;

      case 10:                      // ID
        _senderId[0] = aChar;
        _pos++;
        break;

      case 11:                      // ID
        _senderId[1] = aChar;
        _pos++;
        break;

      case 12:                      // ID
        _senderId[2] = aChar;
        _pos++;
        break;

      case 13:                      // ID
        _senderId[3] = aChar;
        _pos++;
        break;

      case 14:                      // Data
        _payload[0] = aChar;

        _pos++;
        break;

      case 15:                     // Data(4BS)  or crc8(radio) (RPS,1BS)
        _payload[1] = aChar;
        _pos++;
        break;

      case 16:                    // Data(4BS)  or subtelegram count(RPS,1BS)
        _payload[2] = aChar;
        _pos++;
        break;

      case 17:                    // Data(4BS) or RSSI(RPS,1BS)
        if (((_org & 0x0F) == 1) || ((_org & 0x0F) == 0)  ) /* RPS of 1BS Telegram */
        {
          _rssi = aChar ;
          _dataReceived = true;
        }
        else {
          _payload[3] = aChar;
        }

        _pos++;
        break;

      case 18 :              //  crc8(radio) (4BS)
        _pos++;
        break;

      case 19:              //   subtelegram count(4BS)
        _pos++;
        break;

      case 20:              //  RSSI(4BS)
        _rssi = aChar;
        _pos++;
        _dataReceived = true;
        break;

      default:
        //Serial.print("Data: 0x");
        _pos++;

        break;
        //Serial.println(aChar, HEX);
    }


  }

  if (_pos != 0) {
    if (((_org & 0x0F) == 0) || ((_org & 0x0F) == 1) || ((_org & 0x0F) == 2)) { // RPS 1BS 4BS
      prettyPrint();
    }
  }

}




