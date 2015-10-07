unsigned long lastTime=0;
double Input, Output;
double Setpoint=425;

double lastErr;
double kp=0.01;
double ki=0.01;
double kd=0.01;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  analogReadResolution(10);
}

void loop() {
  double errSum;
  // put your main code here, to run repeatedly:
  /*How long since we last calculated*/
  long adcAverage = 0;
  int aveLength=500;
  for(int k=0;k<aveLength;k++){
    Input = analogRead(A4);
    adcAverage+=Input;
  }
  Input = (float)adcAverage/(float)aveLength;
  
   unsigned long now = millis();
   double timeChange = (double)(now - lastTime);
   /*Compute all the working error variables*/
   double error = Setpoint - Input;
   errSum += (error*timeChange);
   double dErr = (error - lastErr) / timeChange;
  
   /*Compute PID Output*/
   Output += kp * error + ki * errSum + kd * dErr;
  
   /*Remember some variables for next time*/
   lastErr = error;
   lastTime = now;

   analogWrite(3,Output);
//Output=1;
//Input=0;
   Serial.print(" error :");
   Serial.print(error);
   Serial.print(" Input :");
   Serial.print(Input);
   Serial.print(" Output :");
   Serial.println(Output);
   //delay(100);
}
