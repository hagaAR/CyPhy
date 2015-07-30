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
//  Temperature = (voltage*6250/165.1)-25;
//  Serial.print(" ; Temperature °C : ");
//  Serial.println(Temperature);
//  //accuracy gap : 0.10 °c
//  return Temperature;
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



void send_SensorData(uint8_t payload_tab[]) {   
  xbee.send(zbTx);
  // after sending a tx request, we expect a status response
  // wait up to half second for the status response
  if (xbee.readPacket(500)) {
    // got a response!
    // should be a znet tx status            	
    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
		  xbee.getResponse().getZBTxStatusResponse(txStatus);
      // get the delivery status, the fifth byte
      if (txStatus.getDeliveryStatus() == SUCCESS) {
        // success.  time to celebrate
		    //Serial.println("success.  time to celebrate");
      } else {
        // the remote XBee did not receive our packet. is it powered on?
        //Serial.println("he remote XBee did not receive our packet. is it powered on?");
      }
    }
  } else if (xbee.getResponse().isError()) {
	    Serial.print("Error reading packet.  Error code: ");
	    Serial.println(xbee.getResponse().getErrorCode());
  } else {
    // local XBee did not provide a timely TX Status Response -- should not happen
    Serial.println("local XBee did not provide a timely TX Status Response -- should not happen");
  }

  //delay(1000);
}



void receive_dataFromRP(){
	xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) {
      // got something
      
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        // got a zb rx packet
        
        // now fill our zb rx class
        xbee.getResponse().getZBRxResponse(rx);
            
        if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
            // the sender got an ACK
    			Serial.println("the sender got an ACK");
          
        } else {
            // we got it (obviously) but sender didn't get an ACK
			Serial.println("we got it (obviously) but sender didn't get an ACK");
        }
        // set dataLed PWM to value of the first byte in the data
        //analogWrite(dataLed, rx.getData(0));
		
		Serial.println("voili voilou");
		for (int i=0;i<rx.getDataLength();i++){
			Serial.print((String)rx.getData(i));
		}
    //Serial.println("");
		Serial.println("voila voila");
		
      } else if (xbee.getResponse().getApiId() == MODEM_STATUS_RESPONSE) {
        xbee.getResponse().getModemStatusResponse(msr);
        // the local XBee sends this response on certain events, like association/dissociation
        
        if (msr.getStatus() == ASSOCIATED) {
          // yay this is great.  flash led
		  Serial.println("yay this is great.");
        } else if (msr.getStatus() == DISASSOCIATED) {
          // this is awful.. flash led to show our discontent
		  Serial.println("this is awful..");
        } else {
          // another status
		  Serial.println("another status");
        }
      } else {
      	// not something we were expecting
		Serial.println("not something we were expecting");		
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
	write_data();
  send_SensorData(payload_SensorData);
  
  //receive_dataFromRP();
  
  delay(1000);
}
