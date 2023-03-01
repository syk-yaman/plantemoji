//LCD imports
#include <SPI.h>
#include "LCD_Driver.h"
#include "LCD_GUI_Paint.h"
#include "LCD_Images.h"
#include <Arduino.h>

//Connectivity imports

//#include <PubSubClient.h>
#include "arduino_secrets.h" // Wifi and MQTT secrets 

//TimeZone import
#include <ezTime.h>

//Constants
const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;   
const char* mqtt_server = "188.34.194.63";

//Plant profile
#include "PlantProfile.h"

//Internal fields
//WiFiClient espClient;
//PubSubClient client(espClient);

int currentMood = -1;
//Timezone GB;

void setup() {
  initialiseLCDScreen();
  Serial.println("Hi");

  //Connect to an SSID and print local IP address, taken from CASA plant monitoring class
  Serial.print("Connecting to ");

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");

  drawMoodOnScreen(1);
}


void loop() {
 
   
}
//Preparing & clearing LCD screen
void initialiseLCDScreen(){
  Config_Init();
  LCD_Init();
  
  LCD_SetBacklight(100);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, BLACK);
  Paint_Clear(BLACK);
}

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
