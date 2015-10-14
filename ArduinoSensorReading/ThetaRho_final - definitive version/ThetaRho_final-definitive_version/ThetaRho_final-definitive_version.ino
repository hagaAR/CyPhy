#include <XBee.h>
#include <PID_v1.h>

int thermocold_pin = A0;
int thermomix_pin = A1;
int thermohot_pin = A2;
int flowmeter1_pin = A3;
int flowmeter2_mix_FS3_pin = A4;
int svalve1_4mm_pin=2;
int svalve2_3mm_pin=3;

double targetted_energy;
String sensorDataStringToSendToRP="";
bool dataToSend=false;
bool openValves=false;
String command_cache = "";
String sensor_cache= "";
String valveAction="";
double targetFlowValue;
int dataTimePeriod_cache =0;
int flowrateValue=0;
char* dataTimePeriodString;
int dataTimeCounter=0;
int dataTimePeriod =0;
int msgCount=0;
boolean receivedActuationCommand=false;

long previousTime=0;

double Setpoint10bit=0;
double Input, Output; //Parameters for PID controller
double Q=0;
double Kp=0.1;
double Ki=1;
double Kd=0;
PID valvePID(&Input, &Output, &Setpoint10bit,Kp,Ki,Kd, DIRECT);

//payload tableau d'octets
uint8_t payload_SensorData[100];
//uint8_t * payload_SensorData;
//allocation dynamique
char payload_reading_data[100];
// create the XBee object // SH + SL Address of receiving XBee
//@ du coordinateur ie raspberry pi
XBee xbee = XBee();
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
  analogWriteResolution(10);
  xbee.setSerial(Serial1);
  openValves=false;
  Input = analogRead(flowmeter2_mix_FS3_pin);
  valvePID.SetMode(AUTOMATIC);

  pinMode(svalve1_4mm_pin,OUTPUT);
  pinMode(svalve2_3mm_pin,OUTPUT);
  
}
 ///
 ///functions to read/set sensors/actuators
double readSensor(int pin) {
  long adcAverage = 0;
  int aveLength=800; //number of AnalogRead in 1 Arduinoloop
  double adcVal;
  double Temperature,flow,result;
  int sensorValue = 0;  // variable to store the value coming from the sensor
  analogReadResolution(10);
  for(int i=0; i<aveLength;i++){
    sensorValue = analogRead(pin);
    adcAverage += sensorValue;
  }
  adcVal=(double)adcAverage/ (double)aveLength;
  double voltage= adcVal * (3.3 / 1023.0);
  //reading PT100
  if( pin==thermocold_pin || pin==thermomix_pin || pin==thermohot_pin ){
    Temperature = (voltage*6250/165.1)-25;
//    SerialUSB.print("Pin: ");
//    SerialUSB.print(String(pin));
//    SerialUSB.print(" Temperature °C: ");
//    SerialUSB.println(Temperature);
    result=Temperature;
  //  //accuracy gap : 0.10 °c    
  }
  //reading flow meter
  if( pin==flowmeter1_pin || pin==flowmeter2_mix_FS3_pin ){
    flow = (1250*voltage/165.1)-5;
//    SerialUSB.print("Pin: ");
//    SerialUSB.print(String(pin));
//    SerialUSB.print(" Value Flow meter : ");
//    SerialUSB.println(flow);
    result=flow;    
  }
  return result;
}

void openValve(double targetted_flow){
 long adcAverage = 0;
 int aveLength=500; //number of AnalogRead in 1 Arduinoloop
 double Temperature,flow,result;
 int thresholdValve1=189;//=759;for 0-1024
 int thresholdValve2=195;//=783;
 analogReadResolution(10);//quantified in 10 bits 0 to1023
 analogWriteResolution(10);
 Setpoint10bit=(targetted_flow+5)*165*1023/(3.3*1250);
 valvePID.SetOutputLimits(785,1024);
 for(int k=0;k<aveLength;k++){
    Input = analogRead(A4);
    adcAverage+=Input;
  }
  Input = (double)adcAverage/(double)aveLength;
  valvePID.Compute();
  analogWrite(svalve2_3mm_pin,Output);
//  SerialUSB.print("Input :");
//  SerialUSB.print(Input);
  SerialUSB.print(" Output :");
  SerialUSB.print(Output);
  SerialUSB.print(" ");
//  
}

double calculatePower(){
  double deltaT,mixFlowValue;
  double Cp=4160.35; // J/(kg.K)
  double P;
  mixFlowValue = readSensor(flowmeter2_mix_FS3_pin); //in L/min
  mixFlowValue=mixFlowValue*0.001/60; //in m3/s
  deltaT = readSensor(thermomix_pin)-readSensor(thermocold_pin); //in K or °C
  P=Cp*deltaT*mixFlowValue; //in W
  P=P/1000;
  return P; // in kW
}
void setValveActuation(){
  double P;
  long currentTime;
  double dt;
  if(openValves){
    P = calculatePower();
    currentTime = millis();
    dt = currentTime-previousTime; //in ms
    dt = dt*0.001/3600; //h
    Q += P*dt; //kW.h
    SerialUSB.print("Q energy : ");
    SerialUSB.println(Q);
    openValve(targetFlowValue);
    previousTime = currentTime;
   if(Q>=targetted_energy){
      closeValve();
      openValves=false;
    SerialUSB.println("closeValve()");
    Q=0;
    }
  }
}

void closeValve(){
  openValve(0);
  analogWrite(svalve2_3mm_pin,0);
}

////
//// functions to send messages to RP
String prepare_send_sensorData_to_RP(int pin){
  String msgToRP = "";
  String sensorDataString="";
  String semicolon =";";
  if(pin==thermocold_pin){
	msgToRP = "thermocold";
	sensorDataString=(String)readSensor(pin);
  }
  if(pin==thermomix_pin){
	msgToRP = "thermomix";
	sensorDataString=(String)readSensor(pin);
  }
  if(pin==thermohot_pin){
	msgToRP = "thermohot";
	sensorDataString=(String)readSensor(pin);
  }
  if(pin==flowmeter1_pin){
	msgToRP = "flowmeter1";
	sensorDataString=(String)readSensor(pin);
  }
  if(pin==flowmeter2_mix_FS3_pin){
	msgToRP = "flowmeter2";
	sensorDataString=(String)readSensor(pin);
  }
  int stringSize = msgToRP.length() + sensorDataString.length() + (String(dataTimeCounter)).length()+semicolon.length()*3;
  char payload_string[stringSize];
  memset(payload_string,0,sizeof(payload_string)); //initialize the array
  strcat(payload_string,msgToRP.c_str());
  strcat(payload_string,";");
  strcat(payload_string,sensorDataString.c_str());
  strcat(payload_string,";");
  strcat(payload_string,(String(dataTimeCounter)).c_str());
  return payload_string;
}
String prepare_send_all_to_RP(){
  String thermocoldmsg, thermomixmsg,thermohotmsg,flowmeter1msg,flowmeter2msg,msgtoSend;
  thermocoldmsg=prepare_send_sensorData_to_RP(thermocold_pin);
  thermomixmsg=prepare_send_sensorData_to_RP(thermomix_pin);
  thermohotmsg=prepare_send_sensorData_to_RP(thermohot_pin);
  flowmeter1msg=prepare_send_sensorData_to_RP(flowmeter1_pin);
  flowmeter2msg=prepare_send_sensorData_to_RP(flowmeter2_mix_FS3_pin);
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
void send_data_to_RP(String message){
  if( dataToSend){
      String payload_charArray;
      payload_charArray=message;
      payload_charArray.concat("/");
      uint8_t payload_bytes[payload_charArray.length()];
      *payload_SensorData=*payload_bytes;
      for(int i=0;i<payload_charArray.length();i++){
        payload_SensorData[i]=(uint8_t)payload_charArray[i];
      dataToSend=false;
      dataTimeCounter=0;
      } 
      xbee.send(zbTx);
     SerialUSB.println("sending to RP... ");
     dataToSend=false;  
    
  }
}

//////////////////////
////////////////////// functions to parse messages from RPi
void receive_message_from_RP(){
  xbee.readPacket();
  
    if (xbee.getResponse().isAvailable()) {
      // got something
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        xbee.getResponse().getZBRxResponse(rx);
        // got a zb rx packet 
        SerialUSB.println("");
        SerialUSB.println("==================received message");
        for (int i=0;i<rx.getDataLength();i++){
          payload_reading_data[i] =(char)rx.getData(i);
        }
      parseMessage();            
      } 
    } else if (xbee.getResponse().isError()) {
    }
}
void parseMessage(){
  SerialUSB.println("parseMessage");
  //Reads data frames from RP
  //frames structures should be like: 'getData;Sensor#1;TimePeriod'
  int commandStringSize=0;
  int commandStringIndex=0;
  int sensorDataStringToSendToRPIndex=0;
  int sensorDataStringToSendToRPSize=0;
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
      sensorDataStringToSendToRPIndex = a;
      break;
    }     
  }
  sensorDataStringToSendToRPSize=sensorDataStringToSendToRPIndex-commandStringSize+1;
  for(int b=sensorDataStringToSendToRPIndex+1;b<sizeof(payload_reading_data);b++){
    if(payload_reading_data[b]==';'){
      dataTimePeriodStringIndex = b;
      break;
    } 
  }
  dataTimePeriodStringSize=dataTimePeriodStringIndex-sensorDataStringToSendToRPIndex-1;
  char tempArr1[commandStringIndex];
  char tempArr2[sensorDataStringToSendToRPSize];
  char tempArr3[dataTimePeriodStringSize];
  for(int k=0;k<commandStringIndex;k++){
    tempArr1[k]=payload_reading_data[k];
  }
  String tempString(tempArr1);
  
  //tempString.substring(0,tempString.length()-2);
  SerialUSB.print("==============tempString: ");
  SerialUSB.println(tempString);
  command_cache="";
  command_cache=tempString;
  
  int count=0;
  for(int g=commandStringIndex+1;g<sensorDataStringToSendToRPIndex;g++){
    tempArr2[count]=payload_reading_data[g];
    count++;
  }
  String tempString2(tempArr2);
  sensor_cache="";
  sensor_cache =tempString2;
  int count2=0;
  for(int c=sensorDataStringToSendToRPIndex+1;c<dataTimePeriodStringIndex;c++){
    tempArr3[count2]=payload_reading_data[c];
    count2++;
  }  
  dataTimePeriod_cache=atoi(tempArr3);
  SerialUSB.print("command_cache: ");
  SerialUSB.println(command_cache);
  SerialUSB.print("sensor_cache: ");
  SerialUSB.println(sensor_cache);
  
}

void read_command(){
  //SerialUSB.println("read_command ");
  //String sensorDataStringToSendToRP="";
  if(strcmp(command_cache.c_str(),"stopAll")==0){
    dataTimePeriod=dataTimePeriod_cache;
    if(strcmp(sensor_cache.c_str(),"all")==0){
      dataTimeCounter =0;
      dataTimePeriod = 0;
      msgCount=0;
      sensorDataStringToSendToRP=prepare_send_stopAll_to_RP();
      dataToSend=false; 
      receivedActuationCommand=false;
      openValves=false;
      SerialUSB.println("Hard Stop!!");
      command_cache="";
      targetted_energy=0;
      sensor_cache="";
    }
  }
  if(strcmp(command_cache.c_str(),"getData")==0){
    dataToSend=true;
    receivedActuationCommand=false;
    
    dataTimePeriod=dataTimePeriod_cache;
	  command_cache="";
  }
  if(strcmp(command_cache.c_str(),"setValv")==0){
   valveAction="setValv";
   receivedActuationCommand=true;
   openValves=true;
   previousTime=millis();
   SerialUSB.println("===setActuator==="); 
    targetFlowValue=(double)dataTimePeriod_cache;
   SerialUSB.print("targetFlowValue: ");
   SerialUSB.println(targetFlowValue);
    char buf[sensor_cache.length()];
    sensor_cache.toCharArray(buf,sensor_cache.length()+1);
    SerialUSB.print("sensor_cache.length(): ");
   SerialUSB.println(sensor_cache.length());
    targetted_energy=atof(buf);
    SerialUSB.print("targetted_energy -------------sensor_cache: ");
   SerialUSB.println(sensor_cache);
    
   SerialUSB.print("targetted_energy: ");
   SerialUSB.println(targetted_energy);
   command_cache="";
  }
}

void read_sensors(){
  if(dataToSend){
    
    receivedActuationCommand=false;
    if(strcmp(sensor_cache.c_str(),"thermocold")==0){
      sensorDataStringToSendToRP=prepare_send_sensorData_to_RP(thermocold_pin);
    }
    ///for thermomix
    if(strcmp(sensor_cache.c_str(),"thermomix")==0){
      sensorDataStringToSendToRP=prepare_send_sensorData_to_RP(thermomix_pin);
    }
    if(strcmp(sensor_cache.c_str(),"thermohot")==0){
      sensorDataStringToSendToRP=prepare_send_sensorData_to_RP(thermohot_pin);
    }
    if(strcmp(sensor_cache.c_str(),"flowmeter1")==0){
      sensorDataStringToSendToRP=prepare_send_sensorData_to_RP(flowmeter1_pin);
    }
    if(strcmp(sensor_cache.c_str(),"flowmeter2")==0){
      sensorDataStringToSendToRP=prepare_send_sensorData_to_RP(flowmeter2_mix_FS3_pin);
    }
    if(strcmp(sensor_cache.c_str(),"all")==0){
      SerialUSB.println("sensor==all"); 
      sensorDataStringToSendToRP=prepare_send_all_to_RP();
    }
    sensor_cache="";
  }
}

////
////main loop
void loop (){
  if(!openValves){
    analogWrite(svalve1_4mm_pin, 0);
    analogWrite(svalve2_3mm_pin, 0);
  }
   
  receive_message_from_RP();
  read_command();
  read_sensors();
  setValveActuation();
  
  if(sensorDataStringToSendToRP!=""){
    send_data_to_RP(sensorDataStringToSendToRP);
	sensorDataStringToSendToRP="";
  }
  //readSensorsCounter();
  
  Serial1.flush();
}

