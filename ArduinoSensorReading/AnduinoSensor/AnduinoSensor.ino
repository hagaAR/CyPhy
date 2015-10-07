
#include <XBee.h>

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

void setup() {
  // start serial
  Serial.begin(9600);
  Serial1.begin(9600);
  xbee.begin(Serial1);

}

// continuously reads packets, looking for ZB Receive or Modem Status
void loop() {
    
    xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) {
      // got something
      
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        // got a zb rx packet
        
        // now fill our zb rx class
        xbee.getResponse().getZBRxResponse(rx);
            
        if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
        
        } else {
           
        }
        // set dataLed PWM to value of the first byte in the data
        //analogWrite(dataLed, rx.getData(0));
        for(int i=0;i<rx.getDataLength();i++){
          Serial.print((char)rx.getData(i));
        }
        
        Serial.println();
      } else if (xbee.getResponse().getApiId() == MODEM_STATUS_RESPONSE) {
        xbee.getResponse().getModemStatusResponse(msr);
        // the local XBee sends this response on certain events, like association/dissociation
        
        if (msr.getStatus() == ASSOCIATED) {
          // yay this is great.  flash led

        } else if (msr.getStatus() == DISASSOCIATED) {
          // this is awful.. flash led to show our discontent

        } else {
          // another status

        }
      } else {
        // not something we were expecting
 
      }
    } else if (xbee.getResponse().isError()) {
      //nss.print("Error reading packet.  Error code: ");  
      //nss.println(xbee.getResponse().getErrorCode());
    }
}
