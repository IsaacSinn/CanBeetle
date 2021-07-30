#include <mcp_can.h>
#include <mcp_can_dfs.h>

#include <mcp_can.h>
#include <mcp_can_dfs.h>

// demo: CAN-BUS Shield, send data
#include <mcp_can.h>
#include <SPI.h>

/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
  #define SERIAL SerialUSB
#else
  #define SERIAL Serial
#endif

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;
const int ledHIGH    = 1;
const int ledLOW     = 0;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup()
{
    SERIAL.begin(115200);

    while (CAN_OK != CAN.begin(CAN_250KBPS, MCP_8MHz))              // init can bus : baudrate = 500k
    {
        SERIAL.println("CAN BUS Shield init fail");
        SERIAL.println(" Init CAN BUS Shield again");
        delay(100);
    }
    SERIAL.println("CAN BUS Shield init ok!");
}

unsigned char stmp[1] = {0xAA};
unsigned char len = 0;
unsigned char buf[8];

void loop()
{   SERIAL.println("In loop");
    // send data:  id = 0x70, standard frame, data len = 8, stmp: data buf
    CAN.sendMsgBuf(0x0FF, 0, 1, stmp);
    delay(1000);                       // send data once per second

    if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

        unsigned long canId = CAN.getCanId();

        SERIAL.println("-----------------------------");
        SERIAL.println("get data from ID: 0x");
        SERIAL.println(canId, HEX);

        for(int i = 0; i<len; i++)    // print the data
        {
            SERIAL.print(buf[i]);
            SERIAL.print("\t");
        }
        SERIAL.println();
    }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
