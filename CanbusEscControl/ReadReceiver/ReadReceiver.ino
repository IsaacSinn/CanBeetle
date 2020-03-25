#include <SPI.h>
#include "mcp_can.h"
#include <Servo.h>

/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
  #define SERIAL SerialUSB
#else
  #define SERIAL Serial
#endif

Servo Esc;

const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);

unsigned char len = 0;
unsigned char msgBuf[8];
unsigned long ID;
int microsecond;
  
void setup()
{
  SERIAL.begin(115200);
  
  Esc.attach(9,1000,2000);
  Esc.writeMicroseconds(1500);
  
  while (CAN_OK != CAN.begin(CAN_500KBPS, MCP_8MHz))
  {
    SERIAL.println("CAN BUS Receive Module init fail");
    SERIAL.println("Retrying...");
    delay(100);
  }
  SERIAL.println("CAN BUS Receive Module init ok!");
}


void loop()
{
  
  if(CAN_MSGAVAIL == CAN.checkReceive())
  {
    CAN.readMsgBuf(&len, msgBuf);
    ID = CAN.getCanId();
    
    // Serial.println("ID\tByte0\tByte1\tMicroseconds");
    SERIAL.print("0x");
    SERIAL.print(ID, HEX);
    SERIAL.print("\t");

    for(int i = 0; i<len; i++)
    {
      SERIAL.print(msgBuf[i]);
      SERIAL.print("\t");
    }
    microsecond = (msgBuf[0] << 8) + msgBuf[1];

    SERIAL.print(microsecond);
    SERIAL.print("\t");

    Esc.writeMicroseconds(microsecond);
    SERIAL.print("ESC written");
    SERIAL.println();
  }
}
