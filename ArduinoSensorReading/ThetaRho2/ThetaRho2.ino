#include <XBee.h>

int sensorPin = A0;    // select the input pin
int sensorValue = 0;  // variable to store the value coming from the sensor
// create the XBee object
XBee xbee = XBee();
//tableau d'octets
//on crée un String de 100octets (limite des pacquets ZigBee?)
uint8_t payload_SensorData[50];
//allocation dynamique
char payload_SensorData_String[100];
char payload_reading_data[100];
bool dataToSend=false;
String command = "";
String sensor= "";
char* dataTimePeriodString;
int dataTimeCounter=0;
int dataTimePeriod =0;
int msgCount=0;
String pending="sending";

// SH + SL Address of receiving XBee
//@ du coordinateur ie raspberry pi
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40c0e60a);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload_SensorData, sizeof(payload_SensorData));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

void setup() {
  Serial.begin(9600);
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
   Serial.println("execute_RP_command ");
    Serial.print("command:");
    Serial.println(command);
   //String str(sensor);
	//Writes msg to RP
	if(strcmp(command.c_str(),"getData")==0){
    Serial.print("command:");
    Serial.println(command);
		//should be a 'switch case'
		if(strcmp(sensor.c_str(),"Thermo1")==0){
			String msgToRP = "Thermo1";
      String PT100_reading;
      char payload_string[100];
      memset(payload_string,0,sizeof(payload_string));
      strcat(payload_string,msgToRP.c_str());
      strcat(payload_string,";");
      strcat(payload_string,readSensorPT100().c_str());
      strcat(payload_string,";");
      strcat(payload_string,(String(dataTimeCounter)).c_str());
      strcat(payload_string,";");

      Serial.print("payload_string:");
      Serial.println(payload_string);
      for(int i=0;i<sizeof(payload_SensorData);i++){
        payload_SensorData[i]=(uint8_t)payload_string[i];
      }

			send_SensorData_to_RP(payload_SensorData);
     Serial.println("sending to RP... ");
		}
	} 

}


void send_SensorData_to_RP(uint8_t payload_tab[]) {   
  xbee.send(zbTx);
  //Serial.print("zbTx.getFrameDataLength():");
  //zbTx.getFrameData();
  //Serial.println(zbTx.getFrameData());
  //zbTx.getFrameDataLength();
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
        parseCommand();    
      } 
    } else if (xbee.getResponse().isError()) {
//			Serial.print("Error reading packet.  Error code: ");
//		  Serial.println(xbee.getResponse().getErrorCode());
    }
}
void parseCommand(){
  
  //Reads data frames from RP
  //frames structures should be like: 'getData;Sensor#1;TimePeriod'
  int commandStringSize=0;
  int commandStringIndex=0;
  int sensorStringIndex=0;
  int sensorStringSize=0;
  int dataTimePeriodStringSize=0;
  int dataTimePeriodStringIndex=0;
   
  Serial.print("payload_reading_data: ");
  Serial.print(payload_reading_data);
  Serial.print(" sizeof(payload_reading_data): ");
  Serial.println(sizeof(payload_reading_data));
  
  for(int i=0;i<sizeof(payload_reading_data);i++){
    if(payload_reading_data[i]==';'){
      commandStringIndex = i;
      Serial.print("commandStringIndex: ");
      Serial.println(commandStringIndex);
      break;
    }     
  }
  commandStringSize=commandStringIndex+1;
  Serial.print("commandStringSize: ");
  Serial.println(commandStringSize);
  for(int a=commandStringSize;a<sizeof(payload_reading_data);a++){
    if(payload_reading_data[a]==';'){
      sensorStringIndex = a;
      Serial.print("sensorStringIndex: ");
      Serial.println(sensorStringIndex);
      break;
    }     
  }
  sensorStringSize=sensorStringIndex-commandStringSize+1;
  Serial.print("sensorStringSize: ");
  Serial.println(sensorStringSize);
  for(int b=sensorStringIndex+1;b<sizeof(payload_reading_data);b++){
    if(payload_reading_data[b]==';'){
      dataTimePeriodStringIndex = b;
      Serial.print("dataTimePeriodStringIndex: ");
      Serial.println(dataTimePeriodStringIndex);
      break;
    } 
  }
  dataTimePeriodStringSize=dataTimePeriodStringIndex-sensorStringIndex-1;
  Serial.print("dataTimePeriodStringSize: ");
  Serial.println(dataTimePeriodStringSize);
  char tempArr1[commandStringSize];
  char tempArr2[sensorStringSize];
  char tempArr3[dataTimePeriodStringSize];
  //command=realloc(command,commandStringSize);
  //char sensor[sensorStringSize];
  //char dataTimePeriodString[dataTimePeriodStringSize];
  Serial.print("payload_reading_data command:");
  for(int k=0;k<commandStringSize-1;k++){
    tempArr1[k]=payload_reading_data[k];
    Serial.print(payload_reading_data[k]);
  }
  String tempString(tempArr1);
  command=tempString;
  Serial.println("|"); 
  Serial.print("payload_reading_data sensor:");
  int count=0;
  for(int g=commandStringIndex+1;g<sensorStringIndex;g++){
    
    tempArr2[count]=payload_reading_data[g];
    Serial.print(payload_reading_data[g]);
    count++;
  }
  String tempString2(tempArr2);
  sensor =tempString2;
  Serial.println("|");
  Serial.print("payload_reading_data dataTimePeriodString:");
  int count2=0;
  for(int c=sensorStringIndex+1;c<dataTimePeriodStringIndex;c++){
    tempArr3[count2]=payload_reading_data[c];
    Serial.print(payload_reading_data[c]);
    count2++;
  }
  //dataTimePeriodString=tempArr3;
  
  dataTimePeriod=atoi(tempArr3);
  Serial.println("|");
  Serial.print("tempArr3: ");
  Serial.println(tempArr3);
  Serial.print("dataTimePeriod: ");
  Serial.println(dataTimePeriod);
//  dataTimePeriod=dataTimePeriodString.toInt();
//  Serial.print("dataTimePeriod: ");
//  Serial.print(dataTimePeriod);
//  Serial.println(" (s)");
  Serial.print("command: ");
  Serial.println(command);
  
  Serial.print("sensor: ");
  Serial.println(sensor);
 dataToSend=true;
 
}
void sendData(){
  if(dataToSend){
    Serial.print(" sendData  command:");
    Serial.println(command);
   if(dataTimeCounter == dataTimePeriod){
    
    Serial.print("finish sending ");
    Serial.print(msgCount);
    Serial.println(" messages!");
    dataTimeCounter =0;
    dataTimePeriod = 0;
  //      command="";
  //      sensor="";
    dataToSend=false;
    msgCount=0;
    return;
   }
//     pending+=".";
//     Serial.print(pending);
     //Send DATA to RP
     execute_RP_command();
     msgCount++;
     dataTimeCounter++;
  }
}

void loop (){
  receive_command_from_RP();
  sendData();
  
  delay(1000);
}
