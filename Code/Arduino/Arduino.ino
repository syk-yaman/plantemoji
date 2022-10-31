const int moisturePin = A5;
const int ldrPin = A4;

void setup()
{
  Serial.begin(115200);
  pinMode(ldrPin, INPUT);
  pinMode(moisturePin, INPUT);
}
void loop()
{
    float soilMoistureValue = analogRead(moisturePin);
    float ldrValue = analogRead(ldrPin);
    String sensorData = String(soilMoistureValue) + ";" + String(1.0) + ";" + String(1.0) + ";" + String(ldrValue);
    Serial.println(sensorData);
    delay(1000);
}
