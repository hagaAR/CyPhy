#include <XBee.h>
// create the XBee object
int Compteur_erreurs=0;
int Compteur_OK=0;
XBee xbee = XBee();
uint8_t payload[] = { 0, 0 };
// SH + SL Address of receiving XBee
//@ du coordinateur
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40c0e60a);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  xbee.setSerial(Serial1);
  Serial.println("on commence");
}

void loop() {   
  payload[0] = 0xfb;
  payload[1] = 0xaf;

  xbee.send(zbTx);
  // after sending a tx request, we expect a status response
  // wait up to half second for the status response
  if (xbee.readPacket(500)) {
    // got a response!
    // should be a znet tx status            	
    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
		  xbee.getResponse().getZBTxStatusResponse(txStatus);
		  //Serial.println("on commence");
      // get the delivery status, the fifth byte
      if (txStatus.getDeliveryStatus() == SUCCESS) {
        // success.  time to celebrate
        Compteur_OK +=1;
		    //Serial.println("success.  time to celebrate");
      } else {
        // the remote XBee did not receive our packet. is it powered on?
        Serial.println("he remote XBee did not receive our packet. is it powered on?");
      }
    }
  } else if (xbee.getResponse().isError()) {
      Compteur_erreurs+=1;
      Serial.print("Nombres erreurs:  ");
      Serial.print(Compteur_erreurs);
      Serial.print(" Nombres OK:  ");
      Serial.print(Compteur_OK);
	    Serial.print(" | Error reading packet.  Error code: ");
	    Serial.println(xbee.getResponse().getErrorCode());
  } else {
    // local XBee did not provide a timely TX Status Response -- should not happen
    Serial.println("local XBee did not provide a timely TX Status Response -- should not happen");
    Compteur_erreurs+=1;
  }

  delay(1000);
}

