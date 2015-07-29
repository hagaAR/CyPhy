#include <XBee.h>
XBee xbee = XBee();
//uint8_t payload[] = { 0, 0 };
//XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40c0e60a);
ZBRxIoSampleResponse ioSample = ZBRxIoSampleResponse();
void setup() { 
  Serial.begin(9600);
  Serial1.begin(9600);
  xbee.setSerial(Serial1);
}

void get_address(ZBRxIoSampleResponse  *ioSample) {
	Serial.print("Received data from address: ");
	Serial.print(ioSample->getRemoteAddress64().getMsb(), HEX);
	Serial.print(ioSample->getRemoteAddress64().getLsb(), HEX);
	Serial.println("");
}

void loop() {
	xbee.readPacket();
	if (xbee.getResponse().isAvailable()) {
  Serial.print("On a recu qch capitaine1");
	if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
		xbee.getResponse().getZBRxIoSampleResponse(ioSample);
		get_address(&ioSample);
		Serial.print("On a recu qch capitaine2");
	}
	else {
		Serial.print("Expected I/O Sample, but got ");
		Serial.print(xbee.getResponse().getApiId(), HEX);
	}
	} else if (xbee.getResponse().isError()) {
		Serial.print("Error reading packet. Error code: ");
		Serial.println(xbee.getResponse().getErrorCode());
	}

  //delay(1000);

}

