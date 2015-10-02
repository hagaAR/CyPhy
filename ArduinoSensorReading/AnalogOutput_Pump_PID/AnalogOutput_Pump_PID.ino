#include <PID_v1.h>
double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint,2,5,1, DIRECT);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(2,OUTPUT);
	Input = analogRead(0);
  Setpoint = 100;
  //turn the PID on
  myPID.SetMode(AUTOMATIC);
}

void loop(){
  Input = analogRead(0);
  myPID.Compute();
  analogWrite(3,Output);
}
