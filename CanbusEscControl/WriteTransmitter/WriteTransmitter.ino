
#include <mcp_can.h>
#include <SPI.h>

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
  #define SERIAL SerialUSB
#else
  #define SERIAL Serial
#endif

const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);

long unsigned int ID = 0x70;
unsigned char msg[2] = {0x05,0xDC};
int len = sizeof(msg);
int input;

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
  if (Serial.available())
  {
  input = Serial.readString().toInt();
    if(input) 
    {
      msg[0] = input >> 8;
      msg[1] = input;
    }
  }
  
  Serial.print(ID, HEX);
  Serial.print("\t");
  Serial.print(input);
  Serial.print("\t");

  for(int i = 0; i<len; i++)
  {
    Serial.print(msg[i]);
    Serial.print("\t");
  }
  
  CAN.sendMsgBuf(ID,0,2,msg);
  
  Serial.println("Msg sent!");
  delay(500);
}
