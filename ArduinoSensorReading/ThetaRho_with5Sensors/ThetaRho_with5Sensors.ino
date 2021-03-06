#include <XBee.h>

int PT100_Thermo1_pin = A0;
int PT100_Thermo2_pin = A1;
int PT100_Thermo3_pin = A2;
int PT100_Thermo4_pin = A3;
int PT100_Thermo5_pin = A4;
int sensorValue = 0;  // variable to store the value coming from the sensor
// create the XBee object
XBee xbee = XBee();
//payload tableau d'octets
uint8_t payload_SensorData[100];
//uint8_t * payload_SensorData;
//allocation dynamique
//char payload_SensorData_String[100];
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
ZBTxRequest zbTx = ZBTxRequest(addr64, payload_SensorData, sizeof(payload_SensorData));;
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

void setup() {
  //SerialUSB.begin(9600);
  Serial1.begin(9600);
  Serial1.flush();
  xbee.setSerial(Serial1);
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
    //if(!dataToSend){
      parseCommand();
    //}
            
      } 
    } else if (xbee.getResponse().isError()) {
//      SerialUSB.print("Error reading packet.  Error code: ");
//      SerialUSB.println(xbee.getResponse().getErrorCode());
    }
}
void parseCommand(){
  SerialUSB.println("parseCommand: ");
  //Reads data frames from RP
  //frames structures should be like: 'getData;Sensor#1;TimePeriod'
  int commandStringSize=0;
  int commandStringIndex=0;
  int sensorStringIndex=0;
  int sensorStringSize=0;
  int dataTimePeriodStringSize=0;
  int dataTimePeriodStringIndex=0;
   
//  SerialUSB.print("payload_reading_data: ");
//  SerialUSB.print(payload_reading_data);
//  SerialUSB.print(" sizeof(payload_reading_data): ");
//  SerialUSB.println(sizeof(payload_reading_data));
  
  for(int i=0;i<sizeof(payload_reading_data);i++){
    if(payload_reading_data[i]==';'){
      commandStringIndex = i;
//      SerialUSB.print("commandStringIndex: ");
//      SerialUSB.println(commandStringIndex);
      break;
    }     
  }
  commandStringSize=commandStringIndex+1;
//  SerialUSB.print("commandStringSize: ");
//  SerialUSB.println(commandStringSize);
  for(int a=commandStringSize;a<sizeof(payload_reading_data);a++){
    if(payload_reading_data[a]==';'){
      sensorStringIndex = a;
//      SerialUSB.print("sensorStringIndex: ");
//      SerialUSB.println(sensorStringIndex);
      break;
    }     
  }
  sensorStringSize=sensorStringIndex-commandStringSize+1;
//  SerialUSB.print("sensorStringSize: ");
//  SerialUSB.println(sensorStringSize);
  for(int b=sensorStringIndex+1;b<sizeof(payload_reading_data);b++){
    if(payload_reading_data[b]==';'){
      dataTimePeriodStringIndex = b;
//      SerialUSB.print("dataTimePeriodStringIndex: ");
//      SerialUSB.println(dataTimePeriodStringIndex);
      break;
    } 
  }
  dataTimePeriodStringSize=dataTimePeriodStringIndex-sensorStringIndex-1;
//  SerialUSB.print("dataTimePeriodStringSize: ");
//  SerialUSB.println(dataTimePeriodStringSize);
  char tempArr1[commandStringSize];
  char tempArr2[sensorStringSize];
  char tempArr3[dataTimePeriodStringSize];
  //command=realloc(command,commandStringSize);
  //char sensor[sensorStringSize];
  //char dataTimePeriodString[dataTimePeriodStringSize];
//  SerialUSB.print("payload_reading_data command:");
  for(int k=0;k<commandStringSize-1;k++){
    tempArr1[k]=payload_reading_data[k];
//    SerialUSB.print(payload_reading_data[k]);
  }
  String tempString(tempArr1);
  command=tempString;
//  SerialUSB.println("|"); 
//  SerialUSB.print("payload_reading_data sensor:");
  int count=0;
  for(int g=commandStringIndex+1;g<sensorStringIndex;g++){
    
    tempArr2[count]=payload_reading_data[g];
    //SerialUSB.print(payload_reading_data[g]);
    count++;
  }
  String tempString2(tempArr2);
  sensor =tempString2;
//  SerialUSB.println("|");
//  SerialUSB.print("payload_reading_data dataTimePeriodString:");
  int count2=0;
  for(int c=sensorStringIndex+1;c<dataTimePeriodStringIndex;c++){
    tempArr3[count2]=payload_reading_data[c];
//    SerialUSB.print(payload_reading_data[c]);
    count2++;
  }
  //dataTimePeriodString=tempArr3;
  
  dataTimePeriod=atoi(tempArr3);
//  SerialUSB.println("|");
//  SerialUSB.print("tempArr3: ");
//  SerialUSB.println(tempArr3);
//  SerialUSB.print("dataTimePeriod: ");
//  SerialUSB.println(dataTimePeriod);
//  dataTimePeriod=dataTimePeriodString.toInt();
//  SerialUSB.print("dataTimePeriod: ");
//  SerialUSB.print(dataTimePeriod);
//  SerialUSB.println(" (s)");
  SerialUSB.print("command: ");
  SerialUSB.println(command);
  SerialUSB.print("sensor: ");
  SerialUSB.println(sensor);
 dataToSend=true;
 
}
String readSensorPT100_Thermo1() {
  long adcAverage = 0;
  int aveLength=800; //number of AnalogRead in 1 Arduinoloop
  float adcVal, volts;
  float Temperature;
  String Temperature_string;
  analogReadResolution(12);
  
  for(int i=0; i<aveLength;i++){
    sensorValue = analogRead(PT100_Thermo1_pin);
    adcAverage += sensorValue;
  }
  adcVal=(float)adcAverage/ (float)aveLength;
  float voltage= adcVal * (3.3 / 4095.0);
  Temperature = (voltage*6250/165.1)-25;
  SerialUSB.print("Thermo1 PT100-Temperature (°C) : ");
  SerialUSB.println(Temperature);
  Temperature_string=(String)Temperature;
//  //accuracy gap : 0.10 °c
  return Temperature_string;
 
}

String readSensorPT100_Thermo2() {
  long adcAverage = 0;
  int aveLength=800; //number of AnalogRead in 1 Arduinoloop
  float adcVal, volts;
  float Temperature;
  String Temperature_string;
  analogReadResolution(12);
  
  for(int i=0; i<aveLength;i++){
    sensorValue = analogRead(PT100_Thermo2_pin);
    adcAverage += sensorValue;
  }
  adcVal=(float)adcAverage/ (float)aveLength;
  float voltage= adcVal * (3.3 / 4095.0);
  Temperature = (voltage*6250/165.1)-25;
  SerialUSB.print("Thermo2 PT100-Temperature (°C) : ");
  SerialUSB.println(Temperature);
  Temperature_string=(String)Temperature;
//  //accuracy gap : 0.10 °c
  return Temperature_string;
  
}

String readSensorPT100_Thermo3() {
  long adcAverage = 0;
  int aveLength=800; //number of AnalogRead in 1 Arduinoloop
  float adcVal, volts;
  float Temperature;
  String Temperature_string;
  analogReadResolution(12);
  
  for(int i=0; i<aveLength;i++){
    sensorValue = analogRead(PT100_Thermo3_pin);
    adcAverage += sensorValue;
  }
  adcVal=(float)adcAverage/ (float)aveLength;
  float voltage= adcVal * (3.3 / 4095.0);
  Temperature = (voltage*6250/165.1)-25;
  SerialUSB.print("Thermo3 PT100-Temperature (°C) : ");
  SerialUSB.println(Temperature);
  Temperature_string=(String)Temperature;
//  //accuracy gap : 0.10 °c
  return Temperature_string;
  
}
String readSensorPT100_Thermo4() {
  long adcAverage = 0;
  int aveLength=800; //number of AnalogRead in 1 Arduinoloop
  float adcVal, volts;
  float Temperature;
  String Temperature_string;
  analogReadResolution(12);
  
  for(int i=0; i<aveLength;i++){
    sensorValue = analogRead(PT100_Thermo4_pin);
    adcAverage += sensorValue;
  }
  adcVal=(float)adcAverage/ (float)aveLength;
  float voltage= adcVal * (3.3 / 4095.0);
  Temperature = (voltage*6250/165.1)-25;
  SerialUSB.print("Thermo4 PT100-Temperature (°C) : ");
  SerialUSB.println(Temperature);
  Temperature_string=(String)Temperature;
//  //accuracy gap : 0.10 °c
  return Temperature_string;
  
}
String readSensorPT100_Thermo5() {
  long adcAverage = 0;
  int aveLength=800; //number of AnalogRead in 1 Arduinoloop
  float adcVal, volts;
  float Temperature;
  String Temperature_string;
  analogReadResolution(12);
  
  for(int i=0; i<aveLength;i++){
    sensorValue = analogRead(PT100_Thermo5_pin);
    adcAverage += sensorValue;
  }
  adcVal=(float)adcAverage/ (float)aveLength;
  float voltage= adcVal * (3.3 / 4095.0);
  Temperature = (voltage*6250/165.1)-25;
  SerialUSB.print("Thermo5 PT100-Temperature (°C) : ");
  SerialUSB.println(Temperature);
  Temperature_string=(String)Temperature;
//  //accuracy gap : 0.10 °c
  return Temperature_string;
  
}
void execute_RP_command(){
   SerialUSB.println("execute_RP_command ");

	if(strcmp(command.c_str(),"stop")==0){
		if(strcmp(sensor.c_str(),"all")==0){
			dataToSend=false;
			SerialUSB.println("Hard Stop!!");
		}
	}
	
	if(strcmp(command.c_str(),"getData")==0){
		if(strcmp(sensor.c_str(),"thermo1")==0){
			send_thermo1_to_RP();
		}
    ///for thermo2
    if(strcmp(sensor.c_str(),"thermo2")==0){
      send_thermo2_to_RP();
    }
		// if(strcmp(sensor.c_str(),"thermo3")==0){
      // send_thermo3_to_RP();
    // }
		// if(strcmp(sensor.c_str(),"thermo4")==0){
      // send_thermo4_to_RP();
    // }
		// if(strcmp(sensor.c_str(),"thermo5")==0){
      // send_thermo5_to_RP();
    // }
		if(strcmp(sensor.c_str(),"all")==0){
			SerialUSB.println("sensor==all");
			send_all_to_RP();
		}
	} 

}

void send_thermo1_to_RP(){
	String msgToRP = "thermo1";
  String PT100_reading;
  String thermo1String;
  String pipe =";";
  thermo1String=readSensorPT100_Thermo1();
  int stringSize = msgToRP.length() + thermo1String.length() + (String(dataTimeCounter)).length()+pipe.length()*3;
  //stringSize+=(String(stringSize)).length();
//      SerialUSB.print("stringSize");
//      SerialUSB.println(stringSize);
  
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string));
  //strcat(payload_string,(String(stringSize)).c_str());
  //strcat(payload_string,";");
  strcat(payload_string,msgToRP.c_str());
  strcat(payload_string,";");
  strcat(payload_string,thermo1String.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  strcat(payload_string,"/");
//
      SerialUSB.print("payload_string:");
      SerialUSB.println(payload_string);
  uint8_t payload_bytes[stringSize];
  *payload_SensorData=*payload_bytes;
      SerialUSB.print("payload_SensorData :");
  for(int i=0;i<stringSize;i++){
    payload_SensorData[i]=(uint8_t)payload_string[i];
        SerialUSB.print(payload_SensorData[i]);
        SerialUSB.print(" ");
  }
  
      SerialUSB.println("");
	send_SensorData_to_RP(payload_SensorData);
 SerialUSB.println("sending to RP... ");
}

void send_thermo2_to_RP(){
	String msgToRP = "thermo2";
  String PT100_reading;
  String pipe =";";
  String thermo2String;
  thermo2String=readSensorPT100_Thermo2();
  int stringSize = msgToRP.length() + thermo2String.length() + (String(dataTimeCounter)).length()+pipe.length()*3;
  //stringSize+=(String(stringSize)).length();
//      SerialUSB.print("stringSize");
//      SerialUSB.println(stringSize);
  
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string));
  //strcat(payload_string,(String(stringSize)).c_str());
  //strcat(payload_string,";");
  strcat(payload_string,msgToRP.c_str());
  strcat(payload_string,";");
  strcat(payload_string,thermo2String.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  strcat(payload_string,"/");
//
  SerialUSB.print("payload_string:");
  SerialUSB.println(payload_string);
  uint8_t payload_bytes[stringSize];
  *payload_SensorData=*payload_bytes;
  SerialUSB.print("payload_SensorData :");
  for(int i=0;i<stringSize;i++){
    payload_SensorData[i]=(uint8_t)payload_string[i];
    SerialUSB.print(payload_SensorData[i]);
    SerialUSB.print(" ");
  }
  
  SerialUSB.println("");
  send_SensorData_to_RP(payload_SensorData);
  SerialUSB.println("sending to RP... ");
}

void send_all_to_RP(){
  String msgToRP1 = "thermo1";
  String msgToRP2 = "thermo2";
  String msgToRP3 = "thermo3";
  String msgToRP4 = "thermo4";
  String msgToRP5 = "thermo5";
  String semiColon =";";
  String comma=",";
  String slash="/";
  String thermo1String;
  String thermo2String;
  String thermo3String;
  String thermo4String;
  String thermo5String;
  thermo1String=readSensorPT100_Thermo1();
  thermo2String=readSensorPT100_Thermo2();
  thermo3String=readSensorPT100_Thermo3();
  thermo4String=readSensorPT100_Thermo4();
  thermo5String=readSensorPT100_Thermo5();
  int stringSize1 = msgToRP1.length() + thermo1String.length() + (String(dataTimeCounter)).length()+semiColon.length()*3;
  int stringSize2 = msgToRP2.length() + thermo2String.length() + (String(dataTimeCounter)).length()+semiColon.length()*3;
  int stringSize3 = msgToRP3.length() + thermo3String.length() + (String(dataTimeCounter)).length()+semiColon.length()*3;
  int stringSize4 = msgToRP4.length() + thermo4String.length() + (String(dataTimeCounter)).length()+semiColon.length()*3;
  int stringSize5 = msgToRP5.length() + thermo5String.length() + (String(dataTimeCounter)).length()+semiColon.length()*3;
  int  stringSize=stringSize1+stringSize2+stringSize3+stringSize4+stringSize5+4*comma.length()+slash.length();

  char payload_string[stringSize];
  
  memset(payload_string,0,sizeof(payload_string));
  //thermo1;35.5;40
  strcat(payload_string,msgToRP1.c_str());
  strcat(payload_string,";");
  strcat(payload_string,thermo1String.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  
  strcat(payload_string,",");
  //thermo1;35.5;40,thermo2;35.5;40
  strcat(payload_string,msgToRP2.c_str());
  strcat(payload_string,";");
  strcat(payload_string,thermo2String.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  
  strcat(payload_string,",");
  //thermo1;35.5;40,thermo2;35.5;40,thermo3;35.5;40
  strcat(payload_string,msgToRP3.c_str());
  strcat(payload_string,";");
  strcat(payload_string,thermo3String.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  
  strcat(payload_string,",");
  //thermo1;35.5;40,thermo2;35.5;40,thermo3;35.5;40,thermo4;35.5;40
  strcat(payload_string,msgToRP4.c_str());
  strcat(payload_string,";");
  strcat(payload_string,thermo4String.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  
  strcat(payload_string,",");
  //thermo1;35.5;40,thermo2;35.5;40,thermo3;35.5;40,thermo4;35.5;40,thermo5;35.5;40
  strcat(payload_string,msgToRP5.c_str());
  strcat(payload_string,";");
  strcat(payload_string,thermo5String.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  //thermo1;35.5;40,thermo2;35.5;40,thermo3;35.5;40,thermo4;35.5;40,thermo5;35.5;40
  strcat(payload_string,"/");

  SerialUSB.print("payload_string:");
  SerialUSB.println(payload_string);
  uint8_t payload_bytes[stringSize];
  *payload_SensorData=*payload_bytes;
  SerialUSB.print("payload_SensorData :");
  for(int i=0;i<stringSize;i++){
    payload_SensorData[i]=(uint8_t)payload_string[i];
    SerialUSB.print(payload_SensorData[i]);
    SerialUSB.print(" ");
  }
  
  SerialUSB.println("");
  send_SensorData_to_RP(payload_SensorData);
  SerialUSB.println("sending to RP... ");
}



void send_SensorData_to_RP(uint8_t payload_tab[]) {   
  xbee.send(zbTx);
  //SerialUSB.print("zbTx.getFrameDataLength():");
  //zbTx.getFrameData();
  //SerialUSB.println(zbTx.getFrameData());
  //zbTx.getFrameDataLength();
}


void sendData(){
  if(dataToSend){
//    SerialUSB.print(" sendData  command:");
//    SerialUSB.println(command);
   if(dataTimeCounter == dataTimePeriod){
    
    SerialUSB.print("finish sending ");
    SerialUSB.print(msgCount);
    SerialUSB.println(" messages!");
    dataTimeCounter =0;
    dataTimePeriod = 0;
  //      command="";
  //      sensor="";
    dataToSend=false;
    msgCount=0;
    return;
   }
//     pending+=".";
//     SerialUSB.print(pending);
     //Send DATA to RP
     execute_RP_command();
     msgCount++;
     dataTimeCounter++;
  }
}

void loop (){
  receive_command_from_RP();
  sendData();
  Serial1.flush();
  delay(1000);
}
