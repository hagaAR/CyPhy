int svalve1_4mm_pin=2;
int svalve2_3mm_pin=3;
void setup() {
  // put your setup code here, to run once:
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
openValveManually50percent();
}
void openValveManually50percent(){
  analogWrite(svalve1_4mm_pin,512);
  analogWrite(svalve2_3mm_pin,512);
}
