
#include <mcp_can.h>
#include <SPI.h>

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
  #define SERIAL SerialUSB
#else
  #define SERIAL Serial
#endif


const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);

long unsigned int ID01 = 0x01;
unsigned char msg01[1] = {0x11};

long unsigned int ID02 = 0x02;
unsigned char msg02[1] = {0x22};

void setup()

{
  SERIAL.begin(115200);

  while (CAN_OK != CAN.begin(CAN_500KBPS, MCP_8MHz))
  {
    SERIAL.println("CAN BUS Transmit Module init fail");
    SERIAL.println("Retrying...");
    delay(100);
  }
  SERIAL.println("CAN BUS Transmit Module init ok!");

  
}

void loop()
{
  Serial.print(ID01, HEX);
  Serial.print("\t");
  Serial.print(msg01[0]);
  Serial.print("\t\t");

  Serial.print(ID02, HEX);
  Serial.print("\t");
  Serial.print(msg02[0]);
  Serial.print("\t");
  
  CAN.sendMsgBuf(ID01,0,1,msg01);
  CAN.sendMsgBuf(ID02,0,1,msg02);
  
  Serial.println("Msg sent!");
  delay(500);
}
