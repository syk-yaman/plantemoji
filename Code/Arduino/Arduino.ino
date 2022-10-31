const int moisturePin = A5;
const int ldrPin = A4;
const int nailsMoisturePin = A3;
const int nailsVCC = A0;

#include <DHT.h>;

//Constants
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

void setup()
{
  Serial.begin(115200);
  pinMode(ldrPin, INPUT);
  pinMode(moisturePin, INPUT);
  pinMode(nailsMoisturePin, INPUT);
  pinMode(nailsVCC, OUTPUT);

  digitalWrite(nailsVCC, HIGH);
  dht.begin();
}
void loop()
{
    float soilMoistureValue = analogRead(moisturePin);
    float ldrValue = analogRead(ldrPin);
    float humidityValue = dht.readHumidity();
    float temperatureValue= dht.readTemperature();

  
    // read the value from the sensor:
    int moisture_val = analogRead(nailsMoisturePin);   // read the resistance      
  
    //float nailSoilMoistureValue = analogRead(nailsMoisturePin);

    String sensorData = String(soilMoistureValue) + ";" + String(temperatureValue) + ";" +
                        String(humidityValue) + ";" + String(ldrValue) + ";" + String(moisture_val);
                        
    Serial.println(sensorData);
    delay(1000);
}
