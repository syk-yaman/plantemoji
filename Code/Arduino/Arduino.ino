const int capacitiveMoisturePin = A5;
const int nailsMoisturePin = A3;
const int nailsVCC = A0;

#include <DHT.h>;

//Constants
#define DHTPIN 7     //DHT pin
#define DHTTYPE DHT22   //DHT type  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

void setup()
{
  Serial.begin(115200);
  pinMode(capacitiveMoisturePin, INPUT);
  pinMode(nailsMoisturePin, INPUT);
  pinMode(nailsVCC, OUTPUT);

  //Nails VCC pin is low in default 
  digitalWrite(nailsVCC, LOW);
  dht.begin();
}
void loop()
{
    //Read sensor data
    int capacitiveMoistureValue = analogRead(capacitiveMoisturePin);
    float humidityValue = dht.readHumidity();
    float temperatureValue= dht.readTemperature();
    int ldrValue = -1; //Cancelled for now
    int nailsMoistureValue = readNailsMoistureValue(nailsMoisturePin);

    //Combine all sensor data to be sent over serial
    String sensorData = String(capacitiveMoistureValue) + ";" + String(temperatureValue) + ";" +
                        String(humidityValue) + ";" + String(ldrValue) + ";" + String(nailsMoistureValue);
                                            
    Serial.println(sensorData);
    delay(2000);
}

int readNailsMoistureValue(int nailsMoisturePin){
   digitalWrite(nailsVCC, HIGH);
   delay(200);
   int nailsMoistureValue = analogRead(nailsMoisturePin);   // read the resistance      
   digitalWrite(nailsVCC, LOW);
   return nailsMoistureValue;
}
