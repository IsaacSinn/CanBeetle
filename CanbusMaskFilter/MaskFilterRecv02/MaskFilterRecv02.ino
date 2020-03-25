#include <SPI.h>
#include "mcp_can.h"
/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
  #define SERIAL SerialUSB
#else
  #define SERIAL Serial
#endif

const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);

unsigned char len = 0;
unsigned char msgBuf[8];
unsigned long ID;
  
void setup()
{
  SERIAL.begin(115200);
  
  while (CAN_OK != CAN.begin(CAN_500KBPS, MCP_8MHz))
  {
    SERIAL.println("CAN BUS Receive Module init fail");
    SERIAL.println("Retrying...");
    delay(100);
  }
  SERIAL.println("CAN BUS Receive Module init ok!");

  CAN.init_Mask(0, 0, 0x3ff); 
  CAN.init_Mask(1, 0, 0x3ff);
  CAN.init_Filt(0, 0, 0x02);
}


void loop()
{
  
  if(CAN_MSGAVAIL == CAN.checkReceive())
  {
    CAN.readMsgBuf(&len, msgBuf);
    ID = CAN.getCanId();
    
    SERIAL.print("0x");
    SERIAL.print(ID, HEX);
    SERIAL.print("\t");
    SERIAL.print(msgBuf[0]);
    SERIAL.print("\t");


    SERIAL.println();
  }
}
