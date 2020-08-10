#define channelA_D 3
#define channelA_A A2

#define channelB_D 11
#define channelB_A A9

#define DEBUG_PRINT

int valueOnOrigin;

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
  pinMode(channelA_D, OUTPUT);
  pinMode(channelA_A, OUTPUT);
  pinMode(channelB_D, OUTPUT);
  pinMode(channelB_A, OUTPUT);
  
}

void loop()
{
  EMon(false, 255);
  delay(2000);
  EMon(false, 80);
  delay(200);
  EMon(false, 127);
  delay(2000);
}
