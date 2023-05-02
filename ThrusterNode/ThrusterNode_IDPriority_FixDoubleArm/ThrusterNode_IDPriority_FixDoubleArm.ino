#include <mcp2515_can.h>
#include <mcp_can.h>

//#define DEBUG_PRINT

#include <DShot.h>

#include <SPI.h>

#include <EEPROM.h>

DShot Esc(DShot::Mode::DSHOT300);

const int SPI_CS_PIN = 10;
mcp2515_can CAN(SPI_CS_PIN);

unsigned char len;
unsigned char msgBuf[8];

byte ID;
byte recID;
int sendID;
byte filtClass;

int PWM;
int controllerInput;
int center = 0;

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
  #ifdef DEBUG_PRINT
  Serial.begin(115200);
  #endif


    
  while (CAN_OK != CAN.begin(CAN_250KBPS, MCP_8MHz))
  {
    debugPrint("CanBeetle initialization failed!!");
    delay(100); 
  }

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);

  Esc.attach(11);
  Esc.setThrottle(center);
  debugPrint("esc attatched");
  
  ID = EEPROM.read(0);
  sendID = ID | 0x100;
  filtClass = (ID & 0xF0) + 0xF;
  
  CAN.init_Mask(0, 0, 0x1FF);
  CAN.init_Mask(1, 0, 0x1FF);
  CAN.init_Filt(0, 0, 0xFF);
  CAN.init_Filt(1, 0, filtClass);
  CAN.init_Filt(2, 0, ID);

  unsigned char msg[8];
  CAN.MCP_CAN::sendMsgBuf(sendID,0,0,msg);
  debugPrint("Connected!");
  newLine();

}

void loop() {
  if(CAN_MSGAVAIL == CAN.checkReceive())
  {
    CAN.readMsgBuf(&len, msgBuf);
    recID = CAN.getCanId();

    if (msgBuf[0] == 0xAA) //roleCall command
    {
      unsigned char msg[8];
      debugPrint("0xAA roleCall: ");
      debugPrint("sendID: ");
      debugPrint("0x");
      debugPrintHex(sendID);
      newLine();
      newLine();
      CAN.MCP_CAN::sendMsgBuf(sendID,0,0,msg);
      
    } else if (msgBuf[0] == 0x20) //dataSend command
    {
      debugPrint("0x20 dataSend: ");
      controllerInput = (msgBuf[1] << 8) + msgBuf[2];
      
      if (controllerInput > 0)
      {
        PWM = map(controllerInput, 1, 32767, 48, 1047);
      } else if (controllerInput < 0)
      {
        PWM = map(controllerInput, -1, -32768, 1048, 2047);
      } else
      {
        PWM = 0;
      }
      
      Esc.setThrottle(PWM);
      
      debugPrint("msg[1]: ");
      debugPrintHex(msgBuf[1]);
      debugPrint(" msg[2]: ");
      debugPrintHex(msgBuf[2]);
      debugPrint(" controllerInput: ");
      debugPrintInt(controllerInput);
      debugPrint(" PWM: ");
      debugPrintInt(PWM);
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

      newLine();
      newLine();
   
      CAN.MCP_CAN::sendMsgBuf(sendID,0,4,msg);
      
    } else if (msgBuf[0] == 0xCE and recID == ID) //changeID command
    {
      debugPrint("0xCE: ");
      
      EEPROM.put(0, msgBuf[1]);
      ID = msgBuf[1];
      sendID = ID | 0x100;

      filtClass = (ID & 0xF0) + 0xF;
      
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
