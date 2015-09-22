#include <XBee.h>

int PT100_thermocold_pin = A0;
int PT100_thermomix_pin = A1;
int PT100_thermohot_pin = A2;
int PT100_flowmeter1_pin = A3;
int PT100_flowmeter2_pin = A4;
int svalve1_pin=2;
int svalve2_pin=3;
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

int opening_percentage=0;

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
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
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
  commandStringSize=commandStringIndex;
//  SerialUSB.print("commandStringSize: ");
//  SerialUSB.println(commandStringSize);
  for(int a=commandStringIndex+1;a<sizeof(payload_reading_data);a++){
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
      //SerialUSB.print("dataTimePeriodStringIndex: ");
      //SerialUSB.println(dataTimePeriodStringIndex);
      break;
    } 
  }
  dataTimePeriodStringSize=dataTimePeriodStringIndex-sensorStringIndex-1;
//  SerialUSB.print("dataTimePeriodStringSize: ");
//  SerialUSB.println(dataTimePeriodStringSize);
  char tempArr1[commandStringIndex];
  char tempArr2[sensorStringSize];
  char tempArr3[dataTimePeriodStringSize];
  //command=realloc(command,commandStringSize);
  //char sensor[sensorStringSize];
  //char dataTimePeriodString[dataTimePeriodStringSize];
  //SerialUSB.print("payload_reading_data command:");
  for(int k=0;k<commandStringIndex;k++){
    tempArr1[k]=payload_reading_data[k];
    //SerialUSB.print(payload_reading_data[k]);
  }
  //SerialUSB.println();

  String tempString(tempArr1);

  command="";
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
  sensor="";
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
  opening_percentage=dataTimePeriod;
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
  float adcVal;
  float Temperature,flow;
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
  //reading PT100
  if( pin==A0 || pin==A1 || pin==A2 ){
	  Temperature = (voltage*6250/165.1)-25;
	  SerialUSB.print("Pin: ");
	  SerialUSB.print(pin);
	  SerialUSB.print(" Value PT100 °C: ");
	  SerialUSB.println(Temperature);
	  Temperature_string=(String)Temperature;
	//  //accuracy gap : 0.10 °c
	  return Temperature_string;
  }
  //reading flow meter
  if( pin==A3 || pin==A4 ){
	  flow = (1250*voltage/165.1)-5;
	  SerialUSB.print("Pin: ");
	  SerialUSB.print(pin);
	  SerialUSB.print(" Value Flow meter : ");
	  SerialUSB.println(flow);
	  Temperature_string=(String)flow;
	//  //accuracy gap : 0.10 °c
	  return Temperature_string;
  }
	return "";

}

void openValve(int valvePin,int openingpercent){
  int openingValue=int(4095*(float)openingpercent/100);
  analogWriteResolution(12);
  analogWrite(valvePin,openingValue);
}


void execute_RP_command(){
  SerialUSB.println("execute_RP_command ");
  String sensorString="";
  if(strcmp(command.c_str(),"stopAll")==0){
    if(strcmp(sensor.c_str(),"all")==0){
      dataTimeCounter =0;
      dataTimePeriod = 0;
      msgCount=0;
      sensorString=prepare_send_stopAll_to_RP();
      dataToSend=false;
      
      SerialUSB.println("Hard Stop!!");
    }
  }
  
  if(strcmp(command.c_str(),"getData")==0){
    if(strcmp(sensor.c_str(),"thermocold")==0){
      sensorString=prepare_send_thermocold_to_RP();
    }
    ///for thermomix
    if(strcmp(sensor.c_str(),"thermomix")==0){
      sensorString=prepare_send_thermomix_to_RP();
    }
    if(strcmp(sensor.c_str(),"thermohot")==0){
      sensorString=prepare_send_thermohot_to_RP();
    }
    if(strcmp(sensor.c_str(),"flowmeter1")==0){
      sensorString=prepare_send_flowmeter1_to_RP();
    }
    if(strcmp(sensor.c_str(),"flowmeter2")==0){
      sensorString=prepare_send_flowmeter2_to_RP();
    }
    if(strcmp(sensor.c_str(),"all")==0){
      SerialUSB.println("sensor==all"); 
      sensorString=prepare_send_all_to_RP();
    }
    if(sensorString!=""){
      send_data_to_RP(sensorString);
    }
  }

	if(strcmp(command.c_str(),"setActuator")==0){
		if(strcmp(sensor.c_str(),"svalve1")==0){
		  openValve(svalve1_pin,opening_percentage);
		  sensorString=prepare_send_svalve1_to_RP();
		}
		///for thermomix
		if(strcmp(sensor.c_str(),"svalve2")==0){
		  openValve(svalve2_pin,opening_percentage);
		  sensorString=prepare_send_svalve2_to_RP();
		}
		if(sensorString!=""){
			send_data_to_RP(sensorString);
		}
	}

}

String prepare_send_thermocold_to_RP(){
  String msgToRP1 = "thermocold";
  String PT100_reading;
  String thermocoldString;
  String semicolon =";";
  thermocoldString=readSensor(PT100_thermocold_pin);
  int stringSize = msgToRP1.length() + thermocoldString.length() + (String(dataTimeCounter)).length()+semicolon.length()*3;
  //stringSize+=(String(stringSize)).length();
//      SerialUSB.print("stringSize");
//      SerialUSB.println(stringSize);
  
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string)); //initialize the array
  //strcat(payload_string,(String(stringSize)).c_str());
  //strcat(payload_string,";");
  strcat(payload_string,msgToRP1.c_str());
  strcat(payload_string,";");
  strcat(payload_string,thermocoldString.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  return payload_string;
}

String prepare_send_thermomix_to_RP(){
  String msgToRP2 = "thermomix";
  String PT100_reading;
  String pipe =";";
  String thermomixString;
  thermomixString=readSensor(PT100_thermomix_pin);
  int stringSize = msgToRP2.length() + thermomixString.length() + (String(dataTimeCounter)).length()+pipe.length()*3;
  //stringSize+=(String(stringSize)).length();
//      SerialUSB.print("stringSize");
//      SerialUSB.println(stringSize);
  
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string));
  //strcat(payload_string,(String(stringSize)).c_str());
  //strcat(payload_string,";");
  strcat(payload_string,msgToRP2.c_str());
  strcat(payload_string,";");
  strcat(payload_string,thermomixString.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  return payload_string;
//
}
String prepare_send_thermohot_to_RP(){
  String msgToRP3 = "thermohot";
  String PT100_reading;
  String pipe =";";
  String thermohotString;
  thermohotString=readSensor(PT100_thermohot_pin);
  int stringSize = msgToRP3.length() + thermohotString.length() + (String(dataTimeCounter)).length()+pipe.length()*3;
  //stringSize+=(String(stringSize)).length();
//      SerialUSB.print("stringSize");
//      SerialUSB.println(stringSize);
  
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string));
  //strcat(payload_string,(String(stringSize)).c_str());
  //strcat(payload_string,";");
  strcat(payload_string,msgToRP3.c_str());
  strcat(payload_string,";");
  strcat(payload_string,thermohotString.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  return payload_string;
//
}
String prepare_send_flowmeter1_to_RP(){
  String msgToRP4 = "flowmeter1";
  String PT100_reading;
  String pipe =";";
  String flowmeter1String;
  flowmeter1String=readSensor(PT100_flowmeter1_pin);
  int stringSize = msgToRP4.length() + flowmeter1String.length() + (String(dataTimeCounter)).length()+pipe.length()*3;
  //stringSize+=(String(stringSize)).length();
//      SerialUSB.print("stringSize");
//      SerialUSB.println(stringSize);
  
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string));
  //strcat(payload_string,(String(stringSize)).c_str());
  //strcat(payload_string,";");
  strcat(payload_string,msgToRP4.c_str());
  strcat(payload_string,";");
  strcat(payload_string,flowmeter1String.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  return payload_string;
//
}
String prepare_send_flowmeter2_to_RP(){
  String msgToRP5 = "flowmeter2";
  String PT100_reading;
  String pipe =";";
  String flowmeter2String;
  flowmeter2String=readSensor(PT100_flowmeter2_pin);
  int stringSize = msgToRP5.length() + flowmeter2String.length() + (String(dataTimeCounter)).length()+pipe.length()*3;
  //stringSize+=(String(stringSize)).length();
//      SerialUSB.print("stringSize");
//      SerialUSB.println(stringSize);
  
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string));
  //strcat(payload_string,(String(stringSize)).c_str());
  //strcat(payload_string,";");
  strcat(payload_string,msgToRP5.c_str());
  strcat(payload_string,";");
  strcat(payload_string,flowmeter2String.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  return payload_string;
//
}

String prepare_send_all_to_RP(){
  String thermocoldmsg, thermomixmsg,thermohotmsg,flowmeter1msg,flowmeter2msg,msgtoSend;
  thermocoldmsg=prepare_send_thermocold_to_RP();
  thermomixmsg=prepare_send_thermomix_to_RP();
  thermohotmsg=prepare_send_thermohot_to_RP();
  flowmeter1msg=prepare_send_flowmeter1_to_RP();
  flowmeter2msg=prepare_send_flowmeter2_to_RP();
  msgtoSend="";
  msgtoSend.concat(thermocoldmsg);
  msgtoSend.concat(",");
  msgtoSend.concat(thermomixmsg);
  msgtoSend.concat(",");
  msgtoSend.concat(thermohotmsg);
  msgtoSend.concat(",");
  msgtoSend.concat(flowmeter1msg);
  msgtoSend.concat(",");
  msgtoSend.concat(flowmeter2msg);
  return msgtoSend;
}

String prepare_send_stopAll_to_RP(){
  String msgToRP1 = "stopGetData;all;";
  int stringSize = msgToRP1.length();
  
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string)); //initialize the array
  strcat(payload_string,msgToRP1.c_str());
  return payload_string;
}

String prepare_send_svalve1_to_RP(){
  String msgToRP1 = "svalve1";
  String opening_percentage1;
  String semicolon =";";
  opening_percentage1=String(opening_percentage);
  int stringSize = msgToRP1.length() + opening_percentage1.length() + (String(dataTimeCounter)).length()+semicolon.length()*3;
  //stringSize+=(String(stringSize)).length();
//      SerialUSB.print("stringSize");
//      SerialUSB.println(stringSize);
  
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string)); //initialize the array
  //strcat(payload_string,(String(stringSize)).c_str());
  //strcat(payload_string,";");
  strcat(payload_string,msgToRP1.c_str());
  strcat(payload_string,";");
  strcat(payload_string,opening_percentage1.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  return payload_string;
}
String prepare_send_svalve2_to_RP(){
  String msgToRP2 = "svalve2";
  String opening_percentage2;
  String semicolon =";";
  opening_percentage2=String(opening_percentage);
  int stringSize = msgToRP2.length() + opening_percentage2.length() + (String(dataTimeCounter)).length()+semicolon.length()*3;
  //stringSize+=(String(stringSize)).length();
//      SerialUSB.print("stringSize");
//      SerialUSB.println(stringSize);
  
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string)); //initialize the array
  //strcat(payload_string,(String(stringSize)).c_str());
  //strcat(payload_string,";");
  strcat(payload_string,msgToRP2.c_str());
  strcat(payload_string,";");
  strcat(payload_string,opening_percentage2.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  return payload_string;
}




void send_data_to_RP(String dataToSend){
  
  //int charArraySize=sizeof(prepare_send_thermocold_to_RP());
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
	opening_percentage=0;
  //      command="";
  //      sensor="";
    dataToSend=false;
    msgCount=0;
    return;
   }
//     pending+=".";
     SerialUSB.print("message ");
     SerialUSB.print(msgCount);
     SerialUSB.print("/");
     SerialUSB.print(dataTimePeriod);
     SerialUSB.println(" sent");
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
  delay(100);
}

