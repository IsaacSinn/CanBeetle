
int channelA_A = 10;
int channelA_B = 11;

void setup() {
  pinMode(channelA_A, OUTPUT);
  pinMode(channelA_B, OUTPUT);
  

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(channelA_A, LOW);
  digitalWrite(channelA_B, HIGH);

}
