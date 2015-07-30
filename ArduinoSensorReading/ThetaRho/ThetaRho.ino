#include <XBee.h>

int sensorPin = A0;    // select the input pin
int sensorValue = 0;  // variable to store the value coming from the sensor
// create the XBee object
XBee xbee = XBee();
//tableau d'octets
//on crée un String de 100octets (limite des pacquets ZigBee?)
uint8_t payload_SensorData[100];
//allocation dynamique
char payload_SensorData_String[100];

// SH + SL Address of receiving XBee
//@ du coordinateur aka raspberry pi
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40c0e60a);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload_SensorData, sizeof(payload_SensorData));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  xbee.setSerial(Serial1);
  Serial.println("let's start");
}

float readSensorPT100() {
  //int sensorValue;
  long adcAverage = 0;
  int aveLength=800; //number of AnalogRead in 1 Arduinoloop
  float adcVal, volts;
  //float Temperature =0;
  float Temperature;
  analogReadResolution(12);
  
  for(int i=0; i<aveLength;i++){
    sensorValue = analogRead(sensorPin);
    adcAverage += sensorValue;
  }
  adcVal=(float)adcAverage/ (float)aveLength;
  float voltage= adcVal * (3.3 / 4095.0);
  
//  Serial.print("voltage ");
//  Serial.print(voltage);
//  Serial.print(" ; analogRead : ");
//  Serial.print(adcVal);
  Temperature = (voltage*6250/165.1)-25;
//  Serial.print(" ; Temperature °C : ");
//  Serial.println(Temperature);
//  //accuracy gap : 0.10 °c
  return Temperature;
  //delay(1000); //1 sec loop
  
}

//void write_data(char string[]){
void write_data(){
  char string[]="hfzsedfz RP, iiukuikm arduinoerztefzgefgyzegfiglgfbegrgergkzerguerugehrgkjzehriuhvuheha fu erfu e rhua u aeu ra ";
  //Serial.println(sizeof(string));
  for(int i=0;i<sizeof(string);i++){
    payload_SensorData_String[i]=string[i];
  }

  //Fill data payload
  //seulement 1 octet (donc seulement 1 caractere)
  if(sizeof(payload_SensorData_String)>100) {
    return;
  }
  for(int i=0;i<sizeof(payload_SensorData_String);i++){
    payload_SensorData[i]=(uint8_t)payload_SensorData_String[i];
  }

  
}


void send_SensorData_toRP(uint8_t payload_tab[]) {   
  xbee.send(zbTx);
}


void receive_dataFromRP(){
	xbee.readPacket(); 
    if (xbee.getResponse().isAvailable()) {
      // got something
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        // got a zb rx packet	
				Serial.println("here is the packet");
				processPayload(rx.getData(),rx.getDataLength());
//				for (int i=0;i<rx.getDataLength();i++){
//					Serial.print((char)rx.getData(i));
//          Serial.print(" ");
//				}
				Serial.println("");
				Serial.println("ending reading");
      } 
    } else if (xbee.getResponse().isError()) {
			Serial.print("Error reading packet.  Error code: ");
			Serial.println(xbee.getResponse().getErrorCode());
    }
}

void loop (){
  uint8_t Temperature=(uint8_t)readSensorPT100();
  //char string[]="hfzsedfz RP, iiukuikm arduino";
  //write_data(write_data(string););

	//write_data();
  //send_SensorData_toRP(payload_SensorData);
  
  receive_dataFromRP();
  
  delay(1000);
}
