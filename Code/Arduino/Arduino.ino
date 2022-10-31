int moisturePin = A5;

void setup()
{
  Serial.begin(115200);

}
void loop()
{
    float soilMoistureValue = analogRead(moisturePin);
    String sensorData = String(soilMoistureValue) + ";" + String(soilMoistureValue) + ";" + String(1.0) + ";" + String(soilMoistureValue);
    Serial.println(sensorData);
    delay(1000);
}
