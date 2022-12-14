//LCD imports
#include <SPI.h>
#include "LCD_Driver.h"
#include "LCD_GUI_Paint.h"
#include "LCD_Images.h"

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
const char* mqtt_server = "mqtt.cetools.org";

//Plant profile
#include "PlantProfile.h"

//Internal fields
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
int currentMood = -1;
Timezone GB;

void setup() {
  initialiseLCDScreen();
  
  Serial.println("Hi");

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
  client.setServer(mqtt_server, 1884);
  client.setCallback(callback); 
  
  pinMode(BUILTIN_LED, OUTPUT);     
  digitalWrite(BUILTIN_LED, HIGH);  
}


void loop() {
  //When a new sensor reading is received from Arduino, it will be parsed and 
  //sent over MQTT, then the LCD screen is updated accordingly
  if (Serial.available() > 0) {
  String sensorDataReceived = "";
  sensorDataReceived = Serial.readString();
  if(sensorDataReceived != ""){
      String soilMoistureReading = splitString(sensorDataReceived,';',0);
      String temperatureReading = splitString(sensorDataReceived,';',1);
      String humidityReading = splitString(sensorDataReceived,';',2);
      String lightReading = splitString(sensorDataReceived,';',3);
      String nailSoilMoistureReading = splitString(sensorDataReceived,';',4);

      Serial.println("------ Arduino said: -------");
      Serial.println("soilMoistureReading: " + soilMoistureReading);
      Serial.println("temperatureReading: " + temperatureReading);
      Serial.println("humidityReading: " + humidityReading);
      Serial.println("lightReading: " + lightReading);
      Serial.println("nailSoilMoistureReading: " + nailSoilMoistureReading);

      int mood = resolveMood(soilMoistureReading.toFloat(), humidityReading.toFloat());
      sendMQTT(soilMoistureReading, temperatureReading, humidityReading, lightReading, nailSoilMoistureReading, mood);
      drawMoodOnScreen(mood);
    }
  
  }
}

// Decide if the plant is happy or sad according to soil moisture and humidity,
// to be developed in the future to include other sensors
int resolveMood(float soilMoistureReading, float humidityReading){
  if(soilMoistureReading > LOWER_MOISTURE_LEVEL)
  {
    return 0;
  }
  if(soilMoistureReading < LOWER_MOISTURE_LEVEL)
  {
    if(humidityReading< LOWER_HUMIDITY_LEVEL)
    {
      return 0;
    }
    return 1;
  }
}

void sendMQTT(String soilMoistureReading, String temperatureReading,
              String humidityReading, String lightReading, String nailSoilMoistureReading, int mood) {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  soilMoistureReading.toCharArray(msg,soilMoistureReading.length());
  client.publish("student/CASA0014/plant/ucfnmyr/moistureCapacitive", msg);

  temperatureReading.toCharArray(msg,temperatureReading.length());
  client.publish("student/CASA0014/plant/ucfnmyr/temperature", msg);

  humidityReading.toCharArray(msg,humidityReading.length());
  client.publish("student/CASA0014/plant/ucfnmyr/humidity", msg);
  
  nailSoilMoistureReading.toCharArray(msg,nailSoilMoistureReading.length());
  client.publish("student/CASA0014/plant/ucfnmyr/moisture", msg);

  sprintf(msg, "%05d", mood);
  client.publish("student/CASA0014/plant/ucfnmyr/mood", msg);
}

//Preparing & clearing LCD screen
void initialiseLCDScreen(){
  Config_Init();
  LCD_Init();
  
  LCD_SetBacklight(100);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, BLACK);
  Paint_Clear(BLACK);
}

//LCD screen output
void drawMoodOnScreen(int mood){

  if(mood == 0 && currentMood != 0)
  {
    currentMood = 0;
    Serial.println("Drawing sad face");
    Paint_Clear(BLACK);
    Paint_DrawImage(gImage_sad, 5, 5, 165, 160); 
  }

  if(mood == 1 && currentMood != 1)
  {
    currentMood = 1;
    Serial.println("Drawing happy face");
    Paint_Clear(BLACK);
    Paint_DrawImage(gImage_happy, 5, 4, 165, 164); 
  } 
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
    if (client.connect(clientId.c_str(), mqttuser, mqttpass)) {
      Serial.println("connected");
      // ... and subscribe to messages on broker
      client.subscribe("student/CASA0014/plant/ucfnmyr/plantemoji");
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

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
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
