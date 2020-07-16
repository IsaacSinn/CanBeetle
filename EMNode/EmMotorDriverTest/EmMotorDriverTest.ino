#define channelA_D 11
#define channelA_A A1

void setup()
{
  pinMode(channelA_D, OUTPUT);
  pinMode(channelA_A, OUTPUT);

}

void loop()
{
  digitalWrite(channelA_D, LOW);
  analogWrite(channelA_A, 255);
  Serial.print("written + ");
  delay(5000);
}
