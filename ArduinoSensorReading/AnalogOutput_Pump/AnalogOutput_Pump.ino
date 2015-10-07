void setup() {
  Serial.begin(9600);
}

void loop(){
   // the default PWM resolution
  analogWriteResolution(10);
  analogWrite(2,0);
  analogWrite(3,1023);

}
