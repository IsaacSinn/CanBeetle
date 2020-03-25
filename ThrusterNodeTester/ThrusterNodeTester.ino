
#include <mcp_can.h>
#include <SPI.h>

const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);

long unsigned int ID = 0xFF;
long unsigned int recID;
unsigned char msg[3] = {};
unsigned char msgBuf[2] = {};
int input = 1500;
unsigned char len;

int roleCall = 0xAA;
int changeID = 0xCE;
int dataSend = 0x20;
int dataRead = 0x10;

void setup()

{
  Serial.begin(115200);

  while (CAN_OK != CAN.begin(CAN_250KBPS, MCP_8MHz))
  {
    Serial.println("CAN BUS Transmit Module init fail");
    Serial.println("Retrying...");
    delay(100);
  }
  Serial.println("CAN BUS Transmit Module init ok!");
}

void loop()
{
  //msg[1] = input >> 8;
  //msg[2] = input;

  msg[0] = 0xAA;
  CAN.sendMsgBuf(ID,0,3,msg);
  Serial.print("0xAA: ");
  Serial.print("roleCall: ");
  if(CAN_MSGAVAIL == CAN.checkReceive())
  {
    CAN.readMsgBuf(&len, msgBuf);
    recID = CAN.getCanId();

    Serial.print("recID: ");
    Serial.print("0x");
    Serial.println(recID,HEX);
  }

  delay(500);
  
  msg[0] = dataSend;
  msg[1] = 0x0;

  Serial.print("0x20: ");
  Serial.print("dataSend: ");
  Serial.print("data: ");
  Serial.print("0x");
  Serial.println(msg[1],HEX);
  CAN.sendMsgBuf(ID,0,3,msg);

  delay(500);
  
  msg[0] = dataRead;
  CAN.sendMsgBuf(ID,0,3,msg);

  Serial.print("0x10: ");
  Serial.print("dataRead: ");
  
  if(CAN_MSGAVAIL == CAN.checkReceive())
  {
    CAN.readMsgBuf(&len, msgBuf);
    recID = CAN.getCanId();

    Serial.print("A0data: ");
    Serial.print("0x");
    Serial.println(msgBuf[0],HEX);
  }
  
  delay(500);

  Serial.print("0xCE: ");
  Serial.print("changeID: ");
  msg[0] = changeID;
  msg[1] = 0x011;
  Serial.print("change to: ");
  Serial.print("0x");
  Serial.println(msg[1],HEX);
  Serial.println();
  

  CAN.sendMsgBuf(ID,0,3,msg);
  


  
}
