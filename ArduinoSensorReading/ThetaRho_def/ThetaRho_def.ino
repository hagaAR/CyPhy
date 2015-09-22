#include <XBee.h>
#include <PID_v1.h>

int PT100_thermocold_pin = A0;
int PT100_thermomix_pin = A1;
int PT100_thermohot_pin = A2;
int flowmeter1_pin = A3;
int flowmeter2_pin = A4;
int svalve1_4mm_pin=2;
int svalve2_3mm_pin=3;
int sensorValue = 0;  // variable to store the value coming from the sensor

double flowSetpoint, Input, Output; //Parameters for PID controller
double Kp=0.1;
double Ki=1;
double Kd=0;
PID valvePID(&Input, &Output, &flowSetpoint,Kp,Ki,Kd, DIRECT);

// create the XBee object
XBee xbee = XBee();
//payload tableau d'octets
uint8_t payload_SensorData[100];
//uint8_t * payload_SensorData;
//allocation dynamique
//char payload_SensorData_String[100];
char payload_reading_data[100];
bool dataToSend=false;
String command_cache = "";
String sensor_cache= "";
String valveAction="";
double targetFlowValue;
int dataTimePeriod_cache =0;
int flowrateValue=0;
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
  
  for(int i=0;i<sizeof(payload_reading_data);i++){
    if(payload_reading_data[i]==';'){
      commandStringIndex = i;
      break;
    }     
  }
  commandStringSize=commandStringIndex;
  for(int a=commandStringIndex+1;a<sizeof(payload_reading_data);a++){
    if(payload_reading_data[a]==';'){
      sensorStringIndex = a;
      break;
    }     
  }
  sensorStringSize=sensorStringIndex-commandStringSize+1;
  for(int b=sensorStringIndex+1;b<sizeof(payload_reading_data);b++){
    if(payload_reading_data[b]==';'){
      dataTimePeriodStringIndex = b;
      break;
    } 
  }
  dataTimePeriodStringSize=dataTimePeriodStringIndex-sensorStringIndex-1;
  char tempArr1[commandStringIndex];
  char tempArr2[sensorStringSize];
  char tempArr3[dataTimePeriodStringSize];
  for(int k=0;k<commandStringIndex;k++){
    tempArr1[k]=payload_reading_data[k];
  }
  String tempString(tempArr1);

  command_cache="";
  command_cache=tempString;
  int count=0;
  for(int g=commandStringIndex+1;g<sensorStringIndex;g++){
    tempArr2[count]=payload_reading_data[g];
    count++;
  }
  String tempString2(tempArr2);
  sensor_cache="";
  sensor_cache =tempString2;
  int count2=0;
  for(int c=sensorStringIndex+1;c<dataTimePeriodStringIndex;c++){
    tempArr3[count2]=payload_reading_data[c];
    count2++;
  }  
  dataTimePeriod_cache=atoi(tempArr3);
  SerialUSB.print("command_cache: ");
  SerialUSB.println(command_cache);
  SerialUSB.print("sensor_cache: ");
  SerialUSB.println(sensor_cache);
 dataToSend=true;
 
}
float readSensor(int pin) {
  long adcAverage = 0;
  int aveLength=800; //number of AnalogRead in 1 Arduinoloop
  float adcVal;
  float Temperature,flow,result;
  analogReadResolution(10);
  
  for(int i=0; i<aveLength;i++){
    sensorValue = analogRead(pin);
    adcAverage += sensorValue;
  }
  adcVal=(float)adcAverage/ (float)aveLength;
  float voltage= adcVal * (3.3 / 1023.0);
  //reading PT100
  if( pin==PT100_thermocold_pin || pin==PT100_thermomix_pin || pin==PT100_thermohot_pin ){
	  Temperature = (voltage*6250/165.1)-25;
	  SerialUSB.print("Pin: ");
	  SerialUSB.print(pin);
	  SerialUSB.print(" Value PT100 °C: ");
	  SerialUSB.println(Temperature);
	  result=Temperature;
	//  //accuracy gap : 0.10 °c	  
  }
  //reading flow meter
  if( pin==flowmeter1_pin || pin==flowmeter2_pin ){
	  flow = (1250*voltage/165.1)-5;
	  SerialUSB.print("Pin: ");
	  SerialUSB.print(pin);
	  SerialUSB.print(" Value Flow meter : ");
	  SerialUSB.println(flow);
	  result=flow;	  
  }
	return result;
}

void openValve(double targetted_flow){
 long adcAverage = 0;
 int aveLength=100; //number of AnalogRead in 1 Arduinoloop
 float adcVal;
 float Temperature,flow,result;
 analogReadResolution(10);
  
  for(int i=0; i<aveLength;i++){
    Input = analogRead(flowmeter1_pin);
    adcAverage += Input;
  }
  Input=(float)adcAverage/ (float)aveLength;
  flowSetpoint =1023*targetted_flow/14.642;
  valvePID.Compute();
  if(targetted_flow<7.0){
    analogWrite(svalve1_4mm_pin,Output);
    analogWrite(svalve2_3mm_pin,Output);
  }
  if(targetted_flow>=7,0 && targetted_flow<7.0){
    analogWrite(svalve1_4mm_pin,Output);
    analogWrite(svalve2_3mm_pin,Output);
  }
  if(targetted_flow>=9.0){
    analogWrite(svalve1_4mm_pin,512);
    analogWrite(svalve2_3mm_pin,Output);
  }
}


void execute_RP_command(){
  SerialUSB.println("execute_RP_command ");
  String sensorString="";
  if(strcmp(command_cache.c_str(),"stopAll")==0){
	  command=command_cache;
	  sensor=sensor_cache;
	  dataTimePeriod=dataTimePeriod_cache;
    if(strcmp(sensor.c_str(),"all")==0){
      dataTimeCounter =0;
      dataTimePeriod = 0;
      msgCount=0;
      sensorString=prepare_send_stopAll_to_RP();
      dataToSend=false;
      
      SerialUSB.println("Hard Stop!!");
    }
  }
  
  if(strcmp(command_cache.c_str(),"getData")==0){
	  command=command_cache;
	  sensor=sensor_cache;
	  dataTimePeriod=dataTimePeriod_cache;
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

	if(strcmp(command_cache.c_str(),"setActuator")==0){
		valveAction="setValve";
		targetFlowValue=(double)dataTimePeriod_cache;
	}

}

String prepare_send_thermocold_to_RP(){
  String msgToRP1 = "thermocold";
  String PT100_reading;
  String thermocoldString;
  String semicolon =";";
  thermocoldString=(String)readSensor(PT100_thermocold_pin);
  int stringSize = msgToRP1.length() + thermocoldString.length() + (String(dataTimeCounter)).length()+semicolon.length()*3;
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string)); //initialize the array
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
  thermomixString=(String)readSensor(PT100_thermomix_pin);
  int stringSize = msgToRP2.length() + thermomixString.length() + (String(dataTimeCounter)).length()+pipe.length()*3;  
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string));
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
  thermohotString=(String)readSensor(PT100_thermohot_pin);
  int stringSize = msgToRP3.length() + thermohotString.length() + (String(dataTimeCounter)).length()+pipe.length()*3;  
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string));
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
  flowmeter1String=(String)readSensor(flowmeter1_pin);
  int stringSize = msgToRP4.length() + flowmeter1String.length() + (String(dataTimeCounter)).length()+pipe.length()*3;
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string));
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
  flowmeter2String=(String)readSensor(flowmeter2_pin);
  int stringSize = msgToRP5.length() + flowmeter2String.length() + (String(dataTimeCounter)).length()+pipe.length()*3;
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string));
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





void send_data_to_RP(String dataToSend){
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
  xbee.send(zbTx);
 SerialUSB.println("sending to RP... ");
}

void sendData(){
  if(dataToSend){
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
  openValve(targetFlowValue);
  Serial1.flush();
  delay(1000);
}

