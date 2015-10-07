#include <PID_v1.h>

//Define Variables we'll be connecting to
//double Setpoint, Input, Output;
double Kp=0.1;//2;
double Ki=1;//5;
double Kd=0;//1
//double Kp=2;
//double Ki=5;
//double Kd=1;

long adcAverage = 0;
 int aveLength=100;
//Define Variables we'll be connecting to
double Setpoint8bit,Setpoint10bit;
double Input, Output;

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint10bit,Kp,Ki,Kd, DIRECT);

void setup()
{
  //initialize the variables we're linked to
  Input = analogRead(A4);
  Serial.begin(9600);
  Setpoint8bit = 96;
   Setpoint10bit = 410;
  //pinMode(2,OUTPUT);
  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  //myPID.SetOutputLimits(0, 1024);
}

void loop()
{
  long adcAverage = 0;
 int aveLength=500; //number of AnalogRead in 1 Arduinoloop
  analogReadResolution(10);//quantified in 10 bits 0 to1023
  analogWriteResolution(10);
  myPID.SetOutputLimits(0,1024);
  for(int k=0;k<aveLength;k++){
    Input = analogRead(A4);
    adcAverage+=Input;
  }
  Input = (float)adcAverage/(float)aveLength;
  
  myPID.Compute();
//  if(Input==0.00 && Output==0.00){
//    loop();
//  }

  analogWrite(3,Output);
  Serial.print("Input :");
  Serial.print(Input);
  Serial.print(" Output :");
  Serial.println(Output);
  
  //delay(1000);
}
