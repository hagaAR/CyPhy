#include <XBee.h>
#include <PID_v1.h>

int PT100_thermocold_pin = A0;
int PT100_thermomix_pin = A1;
int PT100_thermohot_pin = A2;
int flowmeter1_pin = A3;
int flowmeter2_pin = A4;
int valve1_pin =2;
int valve2_pin =3;
int analogRead_value = 0;  // variable to store the value coming from the sensor
// create the XBee object
XBee xbee = XBee();
//payload tableau d'octets
uint8_t payload_SensorData[100];
//allocation dynamique
char payload_reading_data[100];
bool dataToSend=false;
String command = "";
String sensor= "";
char* dataTimePeriodString;
int dataTimeCounter=0;
int dataTimePeriod =0;
int msgCount=0;
String pending="sending";

//Define Variables we'll be connecting to
int Kp=2;int Ki=5;int Kd=1;

double Setpoint, Input, Output;
//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint,Kp,Ki,Kd, DIRECT);
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
	pinMode(valve1_pin,OUTPUT);
  pinMode(valve2_pin,OUTPUT);
	Setpoint=100;
	Input=analogRead(flowmeter1_pin);
	myPID.SetMode(AUTOMATIC);
	analogWriteResolution(12);
	analogReadResolution(12);
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
  command="";
  command=tempString;
  int count=0;
  for(int g=commandStringIndex+1;g<sensorStringIndex;g++){
    tempArr2[count]=payload_reading_data[g];
    count++;
  }
  String tempString2(tempArr2);
  sensor="";
  sensor =tempString2;
  int count2=0;
  for(int c=sensorStringIndex+1;c<dataTimePeriodStringIndex;c++){
    tempArr3[count2]=payload_reading_data[c];
    count2++;
  }
  dataTimePeriod=atoi(tempArr3);
  SerialUSB.print("command: ");
  SerialUSB.println(command);
  SerialUSB.print("sensor: ");
  SerialUSB.println(sensor);
  dataToSend=true;
 
}
float readSensor(int pin) {
  long adcAverage = 0;
  int aveLength=100; //number of AnalogRead in 1 Arduinoloop
  float adcVal, volts;
  float Temperature,flowValue,sensorFloatValue;
  sensorFloatValue=0;
  analogReadResolution(12);
  
  for(int i=0; i<aveLength;i++){
    analogRead_value = analogRead(pin);
    adcAverage += analogRead_value;
  }
  adcVal=(float)adcAverage/ (float)aveLength;
  float voltage= adcVal * (3.3 / 4095.0);
  //reading PT100s
	if(pin==PT100_thermocold_pin || pin==PT100_thermomix_pin || pin==PT100_thermohot_pin){
		Temperature = (voltage*6250/165.1)-25;
		SerialUSB.print("Pin: ");
		SerialUSB.print(pin);
		SerialUSB.print(" Value PT100 °C: ");
		SerialUSB.println(Temperature);
		sensorFloatValue=Temperature;
	//  //accuracy gap : 0.10 °c
	}
	//reading flow meters
	if(pin==flowmeter1_pin || pin==flowmeter2_pin){
	//check resistance with voltmeter!!
		flowValue = (voltage*6250/165.1)-25;
		SerialUSB.print("Pin: ");
		SerialUSB.print(pin);
		SerialUSB.print(" Value flowmeter in lpm: ");
		SerialUSB.println(flowValue);
		sensorFloatValue=flowValue;
	}
  return sensorFloatValue;
}

void setValueOpening(double targetted_flowValue){
	//targetted_flowValue in lpm (Litre per meters)
	double error;
	double mesured_flowValue;
	double error_acceptance;
	double dMax;
  //determiner debit max des valves
	dMax=9;
  error_acceptance= pow(10,-1.0);
	mesured_flowValue=readSensor(flowmeter1_pin);
	error= abs(targetted_flowValue-mesured_flowValue);
	if(error<error_acceptance){
		analogWriteResolution(12);
		if(targetted_flowValue<7.0){
			Input = analogRead(0);
			Setpoint=4095*targetted_flowValue/dMax;
			myPID.Compute();
			analogWrite(3,Output);
			
			analogWrite(valve1_pin,4095);
			analogWrite(valve2_pin,0);
		}
		if(targetted_flowValue>=7.0 || targetted_flowValue<9.0){
			analogWrite(valve1_pin,0);
			analogWrite(valve1_pin,4095);
		}
		if(targetted_flowValue>=9.0){
		}
	}
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
  Serial1.flush();
  delay(1000);
}
