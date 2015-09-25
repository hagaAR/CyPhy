int sensorPin = A2;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor

void setup() {
  Serial.begin(9600);
}

void loop() {
  int sensorValue;
  long adcAverage = 0;
  int aveLength=800; //number of AnalogRead in 1 Arduinoloop
  float adcVal;
  float Temperature =0;
  analogReadResolution(10);
  
  for(int i=0; i<aveLength;i++){
    sensorValue = analogRead(sensorPin);
    adcAverage += sensorValue;
  }
  adcVal=(float)adcAverage/ (float)aveLength;
  float voltage= adcVal * (3.3 / 1023.0);
   Serial.print("adcVal : ");
  Serial.print(adcVal);
  Serial.print(" ; voltage ");
  Serial.print(voltage);
  Serial.print(" ; analogRead : ");
  Serial.print(adcVal);
  Temperature = (voltage*6250/165.1)-25;
  Serial.print(" ; Temperature °C : ");
  Serial.println(Temperature);
  //accuracy gap : 0.10 °c

  delay(1000); //1 sec loop
  
}
