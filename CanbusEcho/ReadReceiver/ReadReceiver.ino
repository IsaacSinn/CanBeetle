#include <SPI.h>
#include "mcp_can.h"

/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
  #define Serial SerialUSB
#else
  #define Serial Serial
#endif

const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);

unsigned char len = 0;
unsigned char msgBuf[8];
unsigned long ID;
int microsecond;
  
void setup()
{
  Serial.begin(115200);
  
  
  while (CAN_OK != CAN.begin(CAN_250KBPS, MCP_8MHz))
  {
    Serial.println("CAN BUS Receive Module init fail");
    Serial.println("Retrying...");
    delay(100);
  }
  Serial.println("CAN BUS Receive Module init ok!");
}


void loop()
{
  
  if(CAN_MSGAVAIL == CAN.checkReceive())
  {
    CAN.readMsgBuf(&len, msgBuf);
    ID = CAN.getCanId();

    CAN.sendMsgBuf(ID, 0, 2, msgBuf);
  }

  
}
