char incomingByte;
void setup() {
	Serial1.begin(9600);
  Serial.begin(9600);
  Serial.println("On commence");
  Serial.write("+++");
  //Serial1.write("+++");
   //establishContact();
}
void loop() {
  //Serial.print("Commence \n");
// make sure everything we need is in the buffer
	//if (Serial1.available()) {
     if (Serial1.read()!=0xffffffff){
       for(int i=0;i<19;i++){
          int incomingByte=Serial1.read();
          Serial.print(incomingByte,HEX);
          Serial.print(" ");    
      	}
       Serial.println();
     }
}

//void establishContact() {
//  while (Serial.available() <= 0) {
//    Serial.print("On commence Serial ");
//    while (Serial1.available() <=0){
//      Serial.print("On commence Serial1 ");   // send a capital A
//      delay(300);
//    }
//  }
//}


/* void setRemoteState(int value) { // pass either a 0x4 or 0x5 to turn the pin on/off
	Serial.print(0x7E, BYTE); // start byte
	Serial.print(0x0, BYTE); // high part of length (always zero)
	Serial.print(0x10, BYTE); // low part of length (the number of bytes that follow, not including checksum)
	Serial.print(0x17, BYTE); // 0x17 is a remote AT command
	Serial.print(0x0, BYTE); // frame id set to zero for no reply
	// ID of recipient, or use 0xFFFF for broadcast
	Serial.print(00, BYTE);
	Serial.print(00, BYTE);
	Serial.print(00, BYTE);
	Serial.print(00, BYTE);
	Serial.print(00, BYTE);
	Serial.print(00, BYTE);
	Serial.print(0xFF, BYTE); // 0xFF for broadcast
	Serial.print(0xFF, BYTE); // 0xFF for broadcast
	// 16 bit of recipient or 0xFFFE if unknown
	Serial.print(0xFF, BYTE);
	Serial.print(0xFE, BYTE);
	Serial.print(0x02, BYTE); // 0x02 to apply changes immediately on remote
	// command name in ASCII characters
	Serial.print('D', BYTE);
	Serial.print('1', BYTE);
	// command data in as many bytes as needed
	Serial.print(value, BYTE);
	// checksum is all bytes after length bytes
	long sum = 0x17 + 0xFF + 0xFF + 0xFF + 0xFE + 0x02 + 'D' + '1' + value;
	Serial.print( 0xFF - ( sum & 0xFF) , BYTE ); // calculate the proper checksum
	delay(10); // safety pause to avoid overwhelming the
	// serial port (if this function is not implemented properly)
} */
