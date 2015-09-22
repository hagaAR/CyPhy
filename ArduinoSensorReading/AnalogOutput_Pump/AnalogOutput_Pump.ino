//int sensorPin=A0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
}

void loop(){
  // read the input on A0 and map it to a PWM pin
  // with an attached LED

    // the default PWM resolution
  analogWriteResolution(12);
  analogWrite(2,4095);
  analogWrite(3,2048);
  
//  int sensorVal = analogRead(A0);
//  Serial.print("Analog Read) : ");
//  //Serial.print(HIGH);
//   Serial.print("  ");
//  Serial.println(sensorVal);

  //delay(1007);
//  analogWrite(2,3500);
//  delay(1007);
}
