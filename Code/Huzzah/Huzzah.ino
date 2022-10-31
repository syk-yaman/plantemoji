#include <SPI.h>
#include "LCD_Driver.h"
#include "LCD_GUI_Paint.h"
#include "LCD_Images.h"

void setup() {

  Config_Init();
  LCD_Init();
  
  LCD_SetBacklight(100);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, BLACK);
  Paint_Clear(BLACK);

  //Paint_DrawImage(gImage_sad, 36, 30, 115, 112);
   
  //Paint_DrawString_EN(30, 10, "123",        &Font24,  YELLOW, RED);  
  //Paint_DrawString_EN(30, 34, "ABC",        &Font24,  BLUE,   CYAN);
  //Paint_SetMirroring(MIRROR_VERTICAL);
//  Paint_DrawFloatNum (30, 58 ,987.654321,3, &Font20,  WHITE,  BLACK);

//Paint_DrawRectangle(125, 10, 225, 58, RED     ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
 //Paint_DrawLine  (125, 10, 225, 58,    MAGENTA ,DOT_PIXEL_2X2,LINE_STYLE_SOLID);
 //Paint_DrawLine  (225, 10, 125, 58,    MAGENTA ,DOT_PIXEL_2X2,LINE_STYLE_SOLID);
// Paint_DrawCircle(220,86,  52,        NEWYELLOW    ,DOT_PIXEL_1X1,DRAW_FILL_FULL);
 //Paint_DrawCircle(180,100,  25,        BLACK   ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
 //Paint_DrawCircle(210,100,  25,        RED     ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
 //Paint_DrawCircle(165,125,  25,        YELLOW  ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
 //Paint_DrawCircle(195,125,  25,        GREEN   ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
 //Paint_DrawImage(gImage_sad, 36, 30, 115, 112);
 
 Serial.println("Hi");

 Serial.println("Drawing happy face");

  delay(500);
  Paint_Clear(BLACK);
  Paint_DrawImage(gImage_happy, 5, 4, 165, 164); 
  
  Serial.println("Drawing sad face");

  delay(500);
  Paint_Clear(BLACK);
  Paint_DrawImage(gImage_sad, 5, 5, 165, 160); 
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

      Serial.println("Arduino said:");
      Serial.println("soilMoistureReading: " + soilMoistureReading);
      Serial.println("temperatureReading: " + temperatureReading);
      Serial.println("humidityReading: " + humidityReading);
      Serial.println("lightReading: " + lightReading);
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
