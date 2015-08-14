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
char payload_reading_data[100];

int dataTimeCounter=0;
int dataTimePeriod =0;
// SH + SL Address of receiving XBee
//@ du coordinateur ie raspberry pi
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40c0e60a);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload_SensorData, sizeof(payload_SensorData));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

void setup() {
  //Serial.begin(9600);
  Serial1.begin(9600);
  xbee.setSerial(Serial1);
}

String readSensorPT100() {
  //int sensorValue;
  long adcAverage = 0;
  int aveLength=800; //number of AnalogRead in 1 Arduinoloop
  float adcVal, volts;
  //float Temperature =0;
  float Temperature;
  String Temperature_string;
  analogReadResolution(12);
  
  for(int i=0; i<aveLength;i++){
    sensorValue = analogRead(sensorPin);
    adcAverage += sensorValue;
  }
  adcVal=(float)adcAverage/ (float)aveLength;
  float voltage= adcVal * (3.3 / 4095.0);
  Temperature = (voltage*6250/165.1)-25;
//  Serial.print(" PT100-Temperature (°C) : ");
//  Serial.println(Temperature);
  Temperature_string=(String)Temperature;
//  //accuracy gap : 0.10 °c
  return Temperature_string;
  //delay(1000); //1 sec loop
  
}

void execute_RP_command(){
    String sensor_reading;
	//Reads data frames from RP
	//frames structures should be like: 'getData;Sensor#1;TimePeriod'
    int commandStringSize=0;
    int sensorStringSize=0;
    for(int i=0;i<sizeof(payload_reading_data);i++){
  		if(payload_reading_data[i]==';'){
  			commandStringSize = i+1;
  			break;
  		} 
    }
    for(int i=commandStringSize-1;i<sizeof(payload_reading_data);i++){
      if(payload_reading_data[i]==';'){
        sensorStringSize = i+1;
        break;
      } 
    }
	char command[commandStringSize];
	char sensor[sensorStringSize];
	for(int k=0;k<commandStringSize-1;k++){
		command[k]=payload_reading_data[k];
	} 

	for(int k=commandStringSize;k<commandStringSize+sensorStringSize-1;k++){
		sensor[k-commandStringSize]=payload_reading_data[k];
	}	
		
	//Writes msg to RP
	if(strcmp(command,"getData")==0){
		//should be a 'switch case'
		if(strcmp(sensor,"Thermo1")==0){
			String msgToRP = "Thermo1;";
			for(int i=0;i<sizeof(msgToRP);i++){
				payload_SensorData[i]=(uint8_t)msgToRP[i];
			}
     sensor_reading=readSensorPT100();
     for(int i=sizeof(msgToRP);i<sizeof(msgToRP)+sizeof(sensor_reading);i++){
       payload_SensorData[i]=(uint8_t)sensor_reading[i-sizeof(msgToRP)];
      }
		 //payload_SensorData[sizeof(msgToRP)]=(uint8_t)readSensorPT100();
     char semicolon= ';';
      payload_SensorData[sizeof(msgToRP)+sizeof(sensor_reading)]=(uint8_t)semicolon; 
			send_SensorData_to_RP(payload_SensorData);
//     Serial.println("sending to RP... ");
		}
	} 
//  Serial.print("command: ");
//  Serial.println(command);
//  Serial.print("sensor: ");
//  Serial.println(sensor);
// 
}

void send_SensorData_to_RP(uint8_t payload_tab[]) {   
  xbee.send(zbTx);
}

void receive_command_from_RP(){
	xbee.readPacket();
    if (xbee.getResponse().isAvailable()) {
      // got something
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        xbee.getResponse().getZBRxResponse(rx);
        // got a zb rx packet	
				for (int i=0;i<rx.getDataLength();i++){
          payload_reading_data[i] =(char)rx.getData(i);
				}
        execute_RP_command();    
      } 
    } else if (xbee.getResponse().isError()) {
//			Serial.print("Error reading packet.  Error code: ");
//		  Serial.println(xbee.getResponse().getErrorCode());
    }
}

void sendData(){
     if(dataTimeCounter == dataTimePeriod){
       
      dataTimeCounter =0;
      dataTimePeriod = 0;
      return;
     }
     //Send DATA to RP
     execute_RP_command();
     dataTimeCounter++;
}

void loop (){
  receive_command_from_RP();
  sendData();
  
  delay(1000);
}
