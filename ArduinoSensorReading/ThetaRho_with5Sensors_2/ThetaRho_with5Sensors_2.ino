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
String readSensor(int pin) {
  long adcAverage = 0;
  int aveLength=800; //number of AnalogRead in 1 Arduinoloop
  float adcVal, volts;
  float Temperature;
  String Temperature_string;
  //String stringPin="PT100_Thermo"+thermo_num+"_pin";
  //int pin=&stringPin;
  analogReadResolution(12);
  
  for(int i=0; i<aveLength;i++){
    sensorValue = analogRead(pin);
    adcAverage += sensorValue;
  }
  adcVal=(float)adcAverage/ (float)aveLength;
  float voltage= adcVal * (3.3 / 4095.0);
  Temperature = (voltage*6250/165.1)-25;
  SerialUSB.print("Pin: ");
  SerialUSB.print(pin);
  SerialUSB.print(" Value PT100 °C: ");
  SerialUSB.println(Temperature);
  Temperature_string=(String)Temperature;
//  //accuracy gap : 0.10 °c
  return Temperature_string;
 
}

void execute_RP_command(){
  SerialUSB.println("execute_RP_command ");
  String sensorString="";
	if(strcmp(command.c_str(),"stop")==0){
		if(strcmp(sensor.c_str(),"all")==0){
			dataToSend=false;
			SerialUSB.println("Hard Stop!!");
		}
	}
	
	if(strcmp(command.c_str(),"getData")==0){
    if(strcmp(sensor.c_str(),"thermo1")==0){
      sensorString=prepare_send_thermo1_to_RP();
    }
    ///for thermo2
    if(strcmp(sensor.c_str(),"thermo2")==0){
      sensorString=prepare_send_thermo2_to_RP();
    }
    if(strcmp(sensor.c_str(),"thermo3")==0){
      sensorString=prepare_send_thermo3_to_RP();
    }
    if(strcmp(sensor.c_str(),"thermo4")==0){
      sensorString=prepare_send_thermo4_to_RP();
    }
    if(strcmp(sensor.c_str(),"thermo5")==0){
      sensorString=prepare_send_thermo5_to_RP();
    }
    if(strcmp(sensor.c_str(),"all")==0){
      SerialUSB.println("sensor==all"); 
      sensorString=prepare_send_all_to_RP();
    }
    if(sensorString!=""){
      send_data_to_RP(sensorString);
    }
	} 

}

String prepare_send_thermo1_to_RP(){
	String msgToRP1 = "thermo1";
  String PT100_reading;
  String thermo1String;
  String semicolon =";";
  thermo1String=readSensor(PT100_Thermo1_pin);
  int stringSize = msgToRP1.length() + thermo1String.length() + (String(dataTimeCounter)).length()+semicolon.length()*3;
  //stringSize+=(String(stringSize)).length();
//      SerialUSB.print("stringSize");
//      SerialUSB.println(stringSize);
  
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string)); //initialize the array
  //strcat(payload_string,(String(stringSize)).c_str());
  //strcat(payload_string,";");
  strcat(payload_string,msgToRP1.c_str());
  strcat(payload_string,";");
  strcat(payload_string,thermo1String.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  return payload_string;
}

String prepare_send_thermo2_to_RP(){
  String msgToRP2 = "thermo2";
  String PT100_reading;
  String pipe =";";
  String thermo2String;
  thermo2String=readSensor(PT100_Thermo2_pin);
  int stringSize = msgToRP2.length() + thermo2String.length() + (String(dataTimeCounter)).length()+pipe.length()*3;
  //stringSize+=(String(stringSize)).length();
//      SerialUSB.print("stringSize");
//      SerialUSB.println(stringSize);
  
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string));
  //strcat(payload_string,(String(stringSize)).c_str());
  //strcat(payload_string,";");
  strcat(payload_string,msgToRP2.c_str());
  strcat(payload_string,";");
  strcat(payload_string,thermo2String.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  return payload_string;
//
}
String prepare_send_thermo3_to_RP(){
  String msgToRP3 = "thermo3";
  String PT100_reading;
  String pipe =";";
  String thermo3String;
  thermo3String=readSensor(PT100_Thermo3_pin);
  int stringSize = msgToRP3.length() + thermo3String.length() + (String(dataTimeCounter)).length()+pipe.length()*3;
  //stringSize+=(String(stringSize)).length();
//      SerialUSB.print("stringSize");
//      SerialUSB.println(stringSize);
  
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string));
  //strcat(payload_string,(String(stringSize)).c_str());
  //strcat(payload_string,";");
  strcat(payload_string,msgToRP3.c_str());
  strcat(payload_string,";");
  strcat(payload_string,thermo3String.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  return payload_string;
//
}
String prepare_send_thermo4_to_RP(){
  String msgToRP4 = "thermo4";
  String PT100_reading;
  String pipe =";";
  String thermo4String;
  thermo4String=readSensor(PT100_Thermo4_pin);
  int stringSize = msgToRP4.length() + thermo4String.length() + (String(dataTimeCounter)).length()+pipe.length()*3;
  //stringSize+=(String(stringSize)).length();
//      SerialUSB.print("stringSize");
//      SerialUSB.println(stringSize);
  
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string));
  //strcat(payload_string,(String(stringSize)).c_str());
  //strcat(payload_string,";");
  strcat(payload_string,msgToRP4.c_str());
  strcat(payload_string,";");
  strcat(payload_string,thermo4String.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  return payload_string;
//
}
String prepare_send_thermo5_to_RP(){
  String msgToRP5 = "thermo5";
  String PT100_reading;
  String pipe =";";
  String thermo5String;
  thermo5String=readSensor(PT100_Thermo5_pin);
  int stringSize = msgToRP5.length() + thermo5String.length() + (String(dataTimeCounter)).length()+pipe.length()*3;
  //stringSize+=(String(stringSize)).length();
//      SerialUSB.print("stringSize");
//      SerialUSB.println(stringSize);
  
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string));
  //strcat(payload_string,(String(stringSize)).c_str());
  //strcat(payload_string,";");
  strcat(payload_string,msgToRP5.c_str());
  strcat(payload_string,";");
  strcat(payload_string,thermo5String.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  return payload_string;
//
}

String prepare_send_all_to_RP(){
  String thermo1msg, thermo2msg,thermo3msg,thermo4msg,thermo5msg,msgtoSend;
  thermo1msg=prepare_send_thermo1_to_RP();
  thermo2msg=prepare_send_thermo2_to_RP();
  thermo3msg=prepare_send_thermo3_to_RP();
  thermo4msg=prepare_send_thermo4_to_RP();
  thermo5msg=prepare_send_thermo5_to_RP();
  msgtoSend="";
  msgtoSend.concat(thermo1msg);
  msgtoSend.concat(",");
  msgtoSend.concat(thermo2msg);
  msgtoSend.concat(",");
  msgtoSend.concat(thermo3msg);
  msgtoSend.concat(",");
  msgtoSend.concat(thermo4msg);
  msgtoSend.concat(",");
  msgtoSend.concat(thermo5msg);
  return msgtoSend;
}

void send_data_to_RP(String dataToSend){
  
  //int charArraySize=sizeof(prepare_send_thermo1_to_RP());
  String payload_charArray;
  payload_charArray=dataToSend;
  payload_charArray.concat("/");
  SerialUSB.print("charArraySize:");
  SerialUSB.println(payload_charArray.length());
  SerialUSB.print("payload_charArray:");
  SerialUSB.println(payload_charArray);
  uint8_t payload_bytes[payload_charArray.length()];
  *payload_SensorData=*payload_bytes;
  SerialUSB.print("payload_SensorData :");
  for(int i=0;i<payload_charArray.length();i++){
    payload_SensorData[i]=(uint8_t)payload_charArray[i];
        SerialUSB.print(payload_SensorData[i]);
        SerialUSB.print(" ");
  }
  
  SerialUSB.println("");
  //send_SensorData_to_RP(payload_SensorData);
  xbee.send(zbTx);
 SerialUSB.println("sending to RP... ");
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
