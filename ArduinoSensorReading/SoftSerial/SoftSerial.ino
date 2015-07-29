void setup()  
{
  // Open serial communications and wait for port to open:
  Serial3.begin(9600);
  Serial3.println("Hello world!");
}

void loop() // run over and over
{
  if (Serial3.available()>0){
    Serial3.println("Serial3 recoit un truc!");
    Serial3.write("Arduino envoie un nouveau truc");
  }

}

