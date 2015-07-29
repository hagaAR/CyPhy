String inputString;
boolean stringComplete=false;
void setup() {
	Serial1.begin(9600);
  Serial.begin(9600);
  inputString.reserve(200);
  Serial.println("On commence");
  //Serial.println("+++");
  Serial1.write("+++");
   //establishContact();
}

void loop() {
if (stringComplete) {
    Serial.println(inputString);
    //Serial.println("ceci est la taille du string: "+ sizeof(inputString));
//    for(int i=0;i<sizeof(inputString)-1;i++){
//      Serial.print(inputString[i]);
//      inputString[i]=0;
//    }
    Serial.println();
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  
}
void serialEvent() {
  while (Serial1.available()) {
    // get the new byte:
//    for(int i=0;i<1;i++){
//      byte discard = Serial1.read();
//    }
    char inChar = (char)Serial1.read();
    //int inChar = Serial1.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
