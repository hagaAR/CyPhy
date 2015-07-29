int inByte = 0;         // incoming serial byte

void setup()
{
  // start serial port at 9600 bps and wait for port to open:
  Serial.begin(9600);
  //establishContact();
}

void loop()
{
  // if we get a valid byte, read analog ins:
//  if (Serial.available() >0) {
//    // get incoming byte:
//    inByte = Serial.read();
//    for(int i=0;i<Serial.available()-1;i++){
//      Serial.print("debut byte ");
//      Serial.print(inByte);
//      Serial.print(" ");
//      //Use USBSerial.print...
//      Serial.write("Envoie un caractere!");
//    }
//    Serial.println();
////    Serial.print(inByte);
////    Serial.write(inByte);
////    Serial.println("  Haga is good");
//  
//  }
  Serial.write("Bonjour Haga");
  delay(1000);
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("Establish Contact");   // send an initial string
    delay(1000);
  }
}
