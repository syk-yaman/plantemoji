#include <SPI.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "image.h"

void setup()
{
  Config_Init();
  LCD_Init();
  
  LCD_SetBacklight(100);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, WHITE);
  Paint_Clear(BLACK);
    
	Paint_DrawString_EN(30, 10, "123",        &Font24,  YELLOW, RED);  
	Paint_DrawString_EN(30, 34, "ABC",        &Font24,  BLUE,   CYAN);
  Paint_SetMirroring(MIRROR_VERTICAL);
//  Paint_DrawFloatNum (30, 58 ,987.654321,3, &Font20,  WHITE,  BLACK);
// Paint_DrawString_CN(250,10, "P",   &Font24CN,WHITE,  RED);
// Paint_DrawString_CN(250,40, "J",   &Font24CN,WHITE,  RED);
// Paint_DrawString_CN(250,70, "K",   &Font24CN,WHITE,  RED);
// Paint_DrawString_CN(250,100, "L",   &Font24CN,WHITE,  RED);

 Paint_DrawRectangle(125, 10, 225, 58, RED     ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
 Paint_DrawLine  (125, 10, 225, 58,    MAGENTA ,DOT_PIXEL_2X2,LINE_STYLE_SOLID);
 Paint_DrawLine  (225, 10, 125, 58,    MAGENTA ,DOT_PIXEL_2X2,LINE_STYLE_SOLID);
 Paint_DrawCircle(150,100,  25,        BLUE    ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
 Paint_DrawCircle(180,100,  25,        BLACK   ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
 Paint_DrawCircle(210,100,  25,        RED     ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
 Paint_DrawCircle(165,125,  25,        YELLOW  ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
 Paint_DrawCircle(195,125,  25,        GREEN   ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
 
 Paint_DrawImage(gImage_70X70, 20, 80, 70, 70); 

  
}
void loop()
{
  
}



/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
