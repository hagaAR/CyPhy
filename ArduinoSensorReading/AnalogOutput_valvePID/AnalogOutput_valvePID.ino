#include <PID_v1.h>

//Define Variables we'll be connecting to
double Setpoint, Input, Output;
double Kp=0.1;
double Ki=1;
double Kd=0;
//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint,Kp,Ki,Kd, DIRECT);

void setup(){
  analogReadResolution(10);
  pinMode(2,OUTPUT);
  //analogWriteResolution(10);
  //initialize the variables we're linked to
  Input = analogRead(A0);
  Setpoint =400;
  //turn the PID on
  myPID.SetMode(AUTOMATIC);
}

void loop(){
  int aveLength=800;
  long adcAverage = 0;
  float adcVal;
  for(int i=0; i<aveLength;i++){
    Input = analogRead(A0);
    adcAverage += Input;
  }
  Input=(float)adcAverage/ (float)aveLength;
  SerialUSB.print("Input: ");
  SerialUSB.println(Input);
  if(Input>390&&Input<450){
    Setpoint =800;
  }
  myPID.Compute();
  analogWrite(2,Output);
  delay(1000);
}
