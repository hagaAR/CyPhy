String inputString = "0";
boolean stringComplete = false ;
//int inByte = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);
  //inputString.reserve(200);
}
void loop() {
  serialEvent();
  //char inputString = (char) Serial2.read();
  if(stringComplete){
    Serial.println(inputString);
  } else{
    Serial2.write("haga haga \n");
  }
  delay(500);
}

void serialEvent(){
  char inputChar = (char) Serial2.read();
  inputString += inputChar;
  if(inputChar =='\n'){
    stringComplete = true;
    inputString="";
  }
}

