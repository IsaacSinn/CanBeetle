
int channelA_A = 3;
int channelA_B = 9;

void setup() {
  pinMode(channelA_A, OUTPUT);
  pinMode(channelA_B, OUTPUT);
  

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(channelA_A, HIGH);
  
  for(int i = 255; i >0; i--) 
  {
    analogWrite(channelA_B, i);
    delay(10);
  }

  digitalWrite(channelA_A, LOW);
  for(int i = 0; i <=255; i++) 
  {
    analogWrite(channelA_B, i);
    delay(10);
  }

}
