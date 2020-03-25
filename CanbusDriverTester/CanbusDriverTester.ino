// demo: CAN-BUS Shield, send data
// loovee@seeed.cc

#include <mcp_can.h>
#include <SPI.h>

/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
  #define SERIAL SerialUSB
#else
  #define SERIAL Serial
#endif

const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);     

unsigned char len;
unsigned char msgBuf[8];
unsigned char stmp[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void setup()
{
    SERIAL.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
        SERIAL.println("CAN BUS Shield init fail");
        SERIAL.println(" Init CAN BUS Shield again");
        delay(500);
    }
    SERIAL.println("CAN BUS Shield init ok!");
}


void loop()
{
  Serial.println("Connected");
  CAN.sendMsgBuf(0x00, 0, 8, stmp);
  delay(1000);

 if(CAN_MSGAVAIL == CAN.checkReceive())
 {
  CAN.readMsgBuf(&len, msgBuf);
  Serial.println(msgBuf[1]);
  
 }
}
