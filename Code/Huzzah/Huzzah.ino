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

const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;   

const char* mqtt_server = "mqtt.cetools.org";

WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];


Timezone GB;

void setup() {
  Config_Init();
  LCD_Init();
  
  LCD_SetBacklight(100);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, BLACK);
  Paint_Clear(BLACK);
  
  Serial.println("Hi");
  Serial.println("Drawing happy face");

  delay(500);
  Paint_Clear(BLACK);
  Paint_DrawImage(gImage_happy, 5, 4, 165, 164); 
  
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

  waitForSync();

  Serial.println("UTC: " + UTC.dateTime());

  GB.setLocation("Europe/London");
  Serial.println("London time: " + GB.dateTime());  

  client.setServer(mqtt_server, 1884);
  client.setCallback(callback); 
  
  
  
  Serial.println("Drawing sad face");

  delay(500);
  Paint_Clear(BLACK);
  Paint_DrawImage(gImage_sad, 5, 5, 165, 160); 

  pinMode(BUILTIN_LED, OUTPUT);     
  digitalWrite(BUILTIN_LED, HIGH);  
}


void loop() {
  //Paint_DrawRectangle(125, 10, 225, 58, RED     ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
  
  if (Serial.available() > 0) {
  String sensorDataReceived = "";
  sensorDataReceived = Serial.readString();
  if(sensorDataReceived != ""){
      String soilMoistureReading = splitString(sensorDataReceived,';',0);
      String temperatureReading = splitString(sensorDataReceived,';',1);
      String humidityReading = splitString(sensorDataReceived,';',2);
      String lightReading = splitString(sensorDataReceived,';',3);

      Serial.println("------ Arduino said: -------");
      Serial.println("soilMoistureReading: " + soilMoistureReading);
      Serial.println("temperatureReading: " + temperatureReading);
      Serial.println("humidityReading: " + humidityReading);
      Serial.println("lightReading: " + lightReading);
      
      sendMQTT(soilMoistureReading, temperatureReading, humidityReading, lightReading);

    }
  
  }
}

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


void sendMQTT(String soilMoistureReading, String temperatureReading, String humidityReading, String lightReading ) {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  soilMoistureReading.toCharArray(msg,soilMoistureReading.length());
  client.publish("student/CASA0014/plant/ucfnmyr/plantemoji/soilMoisture", msg);

  temperatureReading.toCharArray(msg,temperatureReading.length());
  client.publish("student/CASA0014/plant/ucfnmyr/plantemoji/temperature", msg);

  humidityReading.toCharArray(msg,humidityReading.length());
  client.publish("student/CASA0014/plant/ucfnmyr/plantemoji/humidity", msg);

  lightReading.toCharArray(msg,lightReading.length());
  client.publish("student/CASA0014/plant/ucfnmyr/plantemoji/light", msg);

  String happy = "happy!";
  happy.toCharArray(msg, happy.length());
  client.publish("student/CASA0014/plant/ucfnmyr/plantemoji/mood", msg);
}

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
      client.subscribe("student/CASA0014/plant/ucfnmyr/control");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

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
