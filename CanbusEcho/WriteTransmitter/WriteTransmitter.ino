
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
unsigned char msg[1] = {67};
int len = sizeof(msg);
int input;

long unsigned int recID;
unsigned char recmsg[2];
unsigned char reclen;

int startTime;
int endTime;
int totalTime;

void setup()
{
  SERIAL.begin(115200);

  while (CAN_OK != CAN.begin(CAN_250KBPS, MCP_8MHz))
  {
    SERIAL.println("CAN BUS Transmit Module init fail");
    SERIAL.println("Retrying...");
    delay(100);
  }
  SERIAL.println("CAN BUS Transmit Module init ok!");
}

int timeout = 0;

void loop()
{
  startTime = millis();
  
  for(int i = 0;i<100;i++)
  {
    if (Serial.available())
    {
    input = Serial.readString().toInt();
      if(input) 
      {
        msg[0] = input;
      }
    }
    
    CAN.sendMsgBuf(ID,0,2,msg);

    long st = micros();
    long ct = st;
    
    while (CAN_MSGAVAIL != CAN.checkReceive() && ct > st+1000 ){
      ct = micros();
    }

    if (ct>st+1000){
      timeout++;
    }
  
    CAN.readMsgBuf(&reclen, recmsg);
    recID = CAN.getCanId();
    if (recmsg[0] != msg[0])
      timeout++;
    delayMicroseconds(1);
  }
  endTime = millis();
  totalTime = endTime - startTime;
  Serial.print("timeout: ");
  Serial.print(timeout);
  Serial.print("\t");
  Serial.print("totalTime: ");
  Serial.print(totalTime);
  Serial.println("ms");
  
  Serial.print("0x");
  Serial.print(ID, HEX);
  Serial.print("\t");
  Serial.print(msg[0]);
  Serial.print("\t");

  Serial.print("0x");
  Serial.print(recID, HEX);
  Serial.print("\t");
  Serial.print(recmsg[0]);
  Serial.print("\t");

  
}
