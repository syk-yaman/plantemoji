//LCD imports
#include <DHT.h>
#include <DHT_U.h>

//Connectivity imports
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "arduino_secrets.h" // Wifi and MQTT secrets 

//TimeZone import
#include <ezTime.h>

//Constants
const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;   
//const char* mqtt_server = "";

const char* mqtt_server = "";
#define DHTPIN 12 
#define DHTTYPE    DHT22 

DHT_Unified dht(DHTPIN, DHTTYPE);



//Internal fields
WiFiClient espClient;
PubSubClient client(espClient);

int currentMood = -1;
int pumpPin = 13;
int humidifierPin = 12;

Timezone GB;

void setup() {
  Serial.begin(115200);
   pinMode(DHTPIN, INPUT);
   dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  

  //Connect to an SSID and print local IP address, taken from CASA plant monitoring class
  Serial.print("Connecting to ");
  Serial.println(SECRET_SSID);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //Initialise time settings
  waitForSync();
  Serial.println("UTC: " + UTC.dateTime());
  GB.setLocation("Europe/London");
  Serial.println("London time: " + GB.dateTime());  

  //Initialise MQTT connection
  client.setServer(mqtt_server, 9001);
  client.setCallback(callback); 
  
  //drawMoodOnScreen(1);
}


void loop() {
  //When a new sensor reading is received from Arduino, it will be parsed and 
  //sent over MQTT, then the LCD screen is updated accordingly
  sensors_event_t event1;
  dht.temperature().getEvent(&event1);
  if (!isnan(event1.temperature)) {
    Serial.println(event1.temperature);
    
  }
  sensors_event_t event2;
    dht.humidity().getEvent(&event2);
    if (!isnan(event2.relative_humidity)) {
    Serial.println(event2.relative_humidity);
  }
  sendMQTT(String(event1.temperature), String(event2.relative_humidity));
  delay(9000);
}


void sendMQTT(
        String dht22_airTemperature,
        String dht22_airHumidity) {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  char msg[50];
  dht22_airTemperature.toCharArray(msg,dht22_airTemperature.length()+1);
  client.publish("student/CASA0014/plant/ucfnmyr/plantemoji/airTemperatureAux1", msg);

  dht22_airHumidity.toCharArray(msg,dht22_airHumidity.length()+1);
  client.publish("student/CASA0014/plant/ucfnmyr/plantemoji/airHumidityAux1", msg);


}

//MQTT reconnection, taken from CASA plant monitoring class
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {    // while not (!) connected....
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "Plantemoji-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // ... and subscribe to messages on broker
      client.subscribe("student/CASA0014/plant/ucfnmyr/plantemoji/humidifierControl");
      client.subscribe("student/CASA0014/plant/ucfnmyr/plantemoji/pumpControl");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//MQTT callback event, taken from CASA plant monitoring class
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  String convertedTopic = String(topic);
  
  if(convertedTopic.indexOf("pumpControl") != -1)
  {
    if ((char)payload[0] == '1') {
      digitalWrite(pumpPin, HIGH);  
      Serial.println("pump signal to high");
    } else {
      digitalWrite(pumpPin, LOW);
      Serial.println("pump signal to low");
    }
  }  

  if(convertedTopic.indexOf("humidifierControl") != -1)
  {
    if ((char)payload[0] == '1') {
      digitalWrite(humidifierPin, HIGH);  
      Serial.println("humidifier signal to high");
    } else {
      digitalWrite(humidifierPin, LOW);
      Serial.println("humidifier signal to low");
    }
  }  

}

// Useful string splitting function, taken from:
// https://stackoverflow.com/questions/9072320/split-string-into-string-array
String splitString(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
