int sensorPin = A0;    // select the input pin for the potentiometer
//int ledPin = 13;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor

void setup() {
  // declare the ledPin as an OUTPUT:
  SerialUSB.begin(9600);
  //analogReference(INTERNAL);
  //pinMode(ledPin, OUTPUT);
}

void loop() {
  int sensorValue;
  long adcAverage = 0;
  int aveLength=800; //number of AnalogRead in 1 Arduinoloop
  float adcVal, volts;
  float Temperature =0;
  analogReadResolution(12);
  
  for(int i=0; i<aveLength;i++){
    sensorValue = analogRead(sensorPin);
    adcAverage += sensorValue;
  }
  adcVal=(float)adcAverage/ (float)aveLength;
  float voltage= adcVal * (3.3 / 4095.0);
  
  SerialUSB.print("voltage ");
  SerialUSB.print(voltage);
  SerialUSB.print(" ; analogRead : ");
  SerialUSB.print(adcVal);
  Temperature = (voltage*6250/165.1)-25;
  SerialUSB.print(" ; Temperature °C : ");
  SerialUSB.println(Temperature);
  //accuracy gap : 0.10 °c

  delay(1000); //1 sec loop
  
}
