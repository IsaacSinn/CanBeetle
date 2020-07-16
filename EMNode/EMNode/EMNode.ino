#include <mcp_can.h>
#include <mcp_can_dfs.h>

#define DEBUG_PRINT

#include <SPI.h>
#include <EEPROM.h>

#define channelA_D 11
#define channelA_A 9

#define channelB_D 11
#define channelB_A 9

const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);

unsigned char len;
unsigned char msgBuf[8];

int valueOnOrigin;
byte rvsVoltage;
int EMDelay;

byte ID;
byte recID;
int sendID;
byte filtClass;


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

void EMon(bool channel, int value)
{
  //valueOnOrigin = map(value, 0, 255, -255, 255);
  valueOnOrigin = (value-127)*2;
  if(valueOnOrigin>255) {valueOnOrigin = 255;}
  if(channel)
  {
    if(valueOnOrigin == 0)
    {
      digitalWrite(channelA_D, LOW);
      digitalWrite(channelA_A, LOW);
      debugPrint("written A0 ");
    } else if(valueOnOrigin>0)
    {
     digitalWrite(channelA_D, LOW);
     analogWrite(channelA_A, valueOnOrigin);
     debugPrint("written A+ ");
    } else
    {
     digitalWrite(channelA_D, HIGH);
     analogWrite(channelA_A, 255+valueOnOrigin);
     debugPrint("written A- ");
    }
  } else
  {
    if(valueOnOrigin == 0)
    {
      digitalWrite(channelB_D, LOW);
      analogWrite(channelB_A, LOW);
      debugPrint("written B0 ");
    } else if(valueOnOrigin>0)
    {
     digitalWrite(channelB_D, LOW);
     analogWrite(channelB_A, valueOnOrigin);
     debugPrint("written B+ ");
    } else
    {
     digitalWrite(channelB_D, HIGH);
     analogWrite(channelB_A, 255+valueOnOrigin);
     debugPrint("written B- ");
    }
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  delay(2000);
  digitalWrite(5, LOW);

  pinMode(channelA_D, OUTPUT);
  pinMode(channelA_A, OUTPUT);
  pinMode(channelB_D, OUTPUT);
  pinMode(channelB_A, OUTPUT);

  while (CAN_OK != CAN.begin(CAN_250KBPS, MCP_8MHz))
  {
    debugPrint("CanBeetle initialization failed!!");
    delay(100);
  }

  ID = EEPROM.read(0);
  EMDelay = EEPROM.read(1);
  rvsVoltage = EEPROM.read(2);

  sendID = ID | 0x100;
  filtClass = (ID & 0xF0) + 0xF;

  CAN.init_Mask(0, 0, 0x1FF);
  CAN.init_Mask(1, 0, 0x1FF);
  CAN.init_Filt(0, 0, 0xFF);
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
      CAN.sendMsgBuf(sendID,0,0,msg);

    } else if (msgBuf[0] == 0x30) //dataSend EM1
    {
      debugPrint("0x30 dataSend: ");
      debugPrint("msg[1]: ");
      debugPrintHex(msgBuf[1]);
      space();

      if (msgBuf[1] == 1)
      {
        EMon(true, 255);
        debugPrint(" EMon ");
      } else
      {
        EMon(true, rvsVoltage);
        delay(EMDelay);
        EMon(true, 127);
        debugPrint(" EMoff: ");
        debugPrint(" rvsVoltage: ");
        debugPrintInt(rvsVoltage);
        debugPrint(" EMDelay: ");
        debugPrintInt(EMDelay);
      }

      newLine();
      newLine();

    } else if (msgBuf[0] == 0x31) //dataSend EM2
    {
      debugPrint("0x31 dataSend: ");
      debugPrint("msg[1]: ");
      debugPrintHex(msgBuf[1]);

      if (msgBuf[1] == 1)
      {
        EMon(false, 255);
        debugPrint(" EMon ");
      } else
      {
        EMon(false, rvsVoltage);
        delay(EMDelay);
        EMon(false, 127);
        debugPrint(" EMoff:  ");
        debugPrint(" rvsVoltage: ");
        debugPrintInt(rvsVoltage);
        debugPrint(" EMDelay: ");
        debugPrintInt(EMDelay);
      }

      newLine();
      newLine();

    } else if (msgBuf[0] == 0xCD) //change EMDelay
    {
      EEPROM.put(1, msgBuf[1]);
      EMDelay = msgBuf[1];
      debugPrint("0xCD change EMDelay: ");
      debugPrintInt(msgBuf[1]);

      newLine();
      newLine();

    } else if (msgBuf[0] == 0xCC) //change rvsVoltage
    {
      EEPROM.put(2, msgBuf[1]);
      rvsVoltage = msgBuf[1];
      debugPrint("0xCC change rvsVoltage: ");
      debugPrintInt(msgBuf[1]);

      newLine();
      newLine();

    } else if (msgBuf[0] == 0x10) //dataRead command
    {
      unsigned char msg[8];
      debugPrint("0x10 dataRead: ");
      int A0data = analogRead(A0);
      debugPrint(" A0data: ");
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

      CAN.sendMsgBuf(sendID,0,4,msg);

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
