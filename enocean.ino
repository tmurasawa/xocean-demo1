/*************************************************
**              EnOcean Project                 **
**                                              **
** Note: created by K.Konishi , October 2, 2014 **
**       Released into the public domain.       **
*************************************************/

#include <PubSubClient.h>
#include <SPI.h>
#include <Ethernet.h>

#include "EnOceanMsg.h"

EnOceanMsg aMsg;


void setup()
{
  //Pin 0/1
  Serial.begin(57600);              //  EnOcean Serial Protocol 3
  Serial.println("Working");
  aMsg.initeth();
  

}

void loop()
{
  delay(10);
  aMsg.decode();

  if (true == aMsg._dataReceived) {

    aMsg._dataReceived = false;
  }


}
