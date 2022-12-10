void setup() {
  Serial.begin(115200);
}

void loop() {
if (Serial.available() > 0) {
  String sensorDataReceived = "";
  sensorDataReceived = Serial.readString();

   if(sensorDataReceived != ""){
    Serial.println("------ Arduino said: -------");
    Serial.println(sensorDataReceived);
   }
      
}
}