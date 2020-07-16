#define DEBUG_PRINT

#include <SPI.h>
#include "mcp_can.h"
#include <Servo.h>
#include <EEPROM.h>

Servo Esc;

const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);
unsigned char len;

unsigned char msgBuf[8];

byte ID;
int sendID;
byte filtClass;

int PWM;
int controllerInput;


int center;
int E1;
int E2;
unsigned int scaled;

void inline debugPrint(String data)
{
  #ifdef DEBUG_PRINT
  Serial.print(data);
  #endif
}

void inline debugPrintInt(int data) 
{
  #ifdef DEBUG_PRINT
  Serial.print(data);
  #endif
}

void inline debugPrintHex(int data) 
{
  #ifdef DEBUG_PRINT
  Serial.print(data,HEX);
  #endif
}

void inline space() 
{
  #ifdef DEBUG_PRINT
  Serial.print(" ");
  #endif
}

void inline newLine() 
{
  #ifdef DEBUG_PRINT
  Serial.println();
  #endif
}

void setup() 
{
  Serial.begin(115200);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  delay(2000);
  digitalWrite(5, LOW);

  
  while (CAN_OK != CAN.begin(CAN_250KBPS, MCP_8MHz))
  {
    debugPrint("CanBeetle initialization failed!!");
    delay(100);
  }
  
  E1 = EEPROM.read(1);
  E2 = EEPROM.read(2);
  center = (E1 << 8) + E2;
  Esc.attach(11,1000,2000);
  Esc.writeMicroseconds(center);
  debugPrint("esc attatched");
  
  ID = EEPROM.read(0);
  sendID = ID | 0x100;
  filtClass = ID & 0xF0;
  
  CAN.init_Mask(0, 0, 0x7ff);
  CAN.init_Mask(1, 0, 0x7ff);
  CAN.init_Filt(0, 0, 0x000);
  CAN.init_Filt(1, 0, filtClass);
  CAN.init_Filt(2, 0, ID);

  unsigned char msg[8];
  CAN.sendMsgBuf(sendID,0,0,msg);
  debugPrint("Connected!");
  newLine();
}

void loop() {
  if(CAN_MSGAVAIL == CAN.checkReceive())
  {
    CAN.readMsgBuf(&len, msgBuf);
    ID = CAN.getCanId();

    if (msgBuf[0] == 0xAA) //roleCall command
    {
      unsigned char msg[8];
      debugPrint("0xAA roleCall: ");
      debugPrint("sendID: ");
      debugPrint("0x");
      debugPrintHex(sendID);
      newLine();
      newLine();
      CAN.sendMsgBuf(sendID,0,0,msg);
      
    } else if (msgBuf[0] == 0x20) //dataSend command
    {
      debugPrint("0x20 dataSend: ");
      controllerInput = (msgBuf[1] << 8) + msgBuf[2];
      //scaled = controllerInput >> 4;
      //PWM = scaled + 1500;
      scaled = controllerInput/65.534;
      PWM = scaled + center;
      
      debugPrintInt(msgBuf[1]);
      debugPrintInt(msgBuf[2]);
      debugPrint(" controllerInput: ");
      debugPrintInt(controllerInput);
      debugPrint(" scaledValue: ");
      debugPrintInt(scaled);
      debugPrint(" PWM: ");
      debugPrintInt(PWM);
      newLine();
      newLine();
      Esc.writeMicroseconds(PWM);
      
    } else if (msgBuf[0] == 0xCC) //change Center command
    {
      debugPrint("0xCC changeCenter: ");
      center = (msgBuf[1] << 8) + msgBuf[2];
      debugPrint("center: ");
      debugPrintInt(center);
      space();
      E1 = msgBuf[1];
      E2 = msgBuf[2];
      EEPROM.put(1, msgBuf[1]);
      EEPROM.put(2, msgBuf[2]);

      debugPrintInt(EEPROM.read(1));
      space();
      debugPrintInt(EEPROM.read(2));
      newLine();
      newLine();

      
    } else if (msgBuf[0] == 0x10) //dataRead command
    {
      unsigned char msg[8];
      debugPrint("0x10 dataRead: ");
      int A0data = analogRead(A0);
      debugPrint("A0data: ");
      debugPrintInt(A0data);
      msg[0] = A0data >> 8;
      msg[1] = A0data;

      int A1data = analogRead(A1);
      debugPrint(" A1data: ");
      debugPrintInt(A1data);
      msg[2] = A1data >> 8;
      msg[3] = A1data;

      msg[4] = E1;
      msg[5] = E2;
      debugPrint(" center: ");
      debugPrintInt(center);
      newLine();
      newLine();
      
      CAN.sendMsgBuf(sendID,0,6,msg);
      
    } else if (msgBuf[0] == 0xCE) //chnageID command
    {
      debugPrint("0xCE: ");
      
      EEPROM.put(0, msgBuf[1]);
      ID = msgBuf[1];
      sendID = ID | 0x100;

      filtClass = ID & 0xF0;
      
      CAN.init_Filt(1,0, filtClass);
      CAN.init_Filt(2, 0, ID);

      debugPrint("filtClass: ");
      debugPrint("0x");
      debugPrintHex(filtClass);
      debugPrint(" ID: ");
      debugPrint(" 0x");
      debugPrintHex(ID);
      newLine();
      newLine();
    }
  }

}
