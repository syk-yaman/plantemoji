/*************************************************** 
  This is a library for the Si1145 UV/IR/Visible Light Sensor

  Designed specifically to work with the Si1145 sensor in the
  adafruit shop
  ----> https://www.adafruit.com/products/1777

  These sensors use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#define SI1145_ADDR 0x60

#define SI1145_REG_PARTID 0x00

#define SI1145_REG_UCOEFF0 0x13
#define SI1145_REG_UCOEFF1 0x14
#define SI1145_REG_UCOEFF2 0x15
#define SI1145_REG_UCOEFF3 0x16

//-------------------------------
#define SI1145_REG_INTCFG 0x03
#define SI1145_REG_INTCFG_INTOE 0x01
#define SI1145_REG_INTCFG_INTMODE 0x02

#define SI1145_REG_IRQEN 0x04
#define SI1145_REG_IRQEN_ALSEVERYSAMPLE 0x01
#define SI1145_REG_IRQEN_PS1EVERYSAMPLE 0x04
#define SI1145_REG_IRQEN_PS2EVERYSAMPLE 0x08
#define SI1145_REG_IRQEN_PS3EVERYSAMPLE 0x10

/* Parameters */
#define SI1145_PARAM_I2CADDR 0x00
#define SI1145_PARAM_CHLIST 0x01
#define SI1145_PARAM_CHLIST_ENUV 0x80
#define SI1145_PARAM_CHLIST_ENAUX 0x40
#define SI1145_PARAM_CHLIST_ENALSIR 0x20
#define SI1145_PARAM_CHLIST_ENALSVIS 0x10
#define SI1145_PARAM_CHLIST_ENPS1 0x01
#define SI1145_PARAM_CHLIST_ENPS2 0x02
#define SI1145_PARAM_CHLIST_ENPS3 0x04


#define SI1145_REG_MEASRATE0 0x08
#define SI1145_REG_COMMAND 0x18
#define SI1145_PSALS_AUTO 0x0F

#define SI1145_REG_IRQMODE1 0x05
#define SI1145_REG_IRQMODE2 0x06

#define SI1145_RESET 0x01
#define SI1145_REG_HWKEY 0x07
#define SI1145_REG_MEASRATE0 0x08
#define SI1145_REG_MEASRATE1 0x09
#define SI1145_REG_PSRATE 0x0A

#define SI1145_REG_IRQSTAT 0x21
#define SI1145_REG_IRQSTAT_ALS 0x01

void setup() {
  Serial.begin(9600);
  Wire.begin(SI1145_ADDR); 
  Serial.println("Adafruit SI1145 test");

  Wire.beginTransmission(SI1145_ADDR);
  Wire.write(SI1145_REG_PARTID);

  while (Wire.available()) { // peripheral may send less than requested
    char c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }

  //RESET
  Wire.write(SI1145_REG_MEASRATE0);
  Wire.write(0);
  Wire.write(SI1145_REG_MEASRATE1);
  Wire.write(0);
  Wire.write(SI1145_REG_IRQEN);
  Wire.write(0);
  Wire.write(SI1145_REG_IRQMODE1);
  Wire.write(0);
  Wire.write(SI1145_REG_IRQMODE2);
  Wire.write(0);
  Wire.write(SI1145_REG_INTCFG);
  Wire.write(0);
  Wire.write(SI1145_REG_IRQSTAT);
  Wire.write(0xFF);
  Wire.write(SI1145_REG_COMMAND);
  Wire.write(SI1145_RESET);

  delay(100);
  Wire.write(SI1145_REG_HWKEY);
  Wire.write(0x17);

  delay(100);
  //END RESET

  
  // enable UVindex measurement coefficients!
  Wire.write(SI1145_REG_UCOEFF0);
  Wire.write(0x29);
  Wire.write(SI1145_REG_UCOEFF1);
  Wire.write(0x89);
  Wire.write(SI1145_REG_UCOEFF2);
  Wire.write(0x02);
  Wire.write(SI1145_REG_UCOEFF3);
  Wire.write(0x00);
  
  // enable UV sensor
  Wire.write(SI1145_PARAM_CHLIST);
  Wire.write(SI1145_PARAM_CHLIST_ENUV | SI1145_PARAM_CHLIST_ENALSIR |
                 SI1145_PARAM_CHLIST_ENALSVIS | SI1145_PARAM_CHLIST_ENPS1);

  // enable interrupt on every sample
  //Wire.write(SI1145_REG_INTCFG);
  //Wire.write(SI1145_REG_INTCFG_INTOE);
  //Wire.write(SI1145_REG_IRQEN);
  //Wire.write(SI1145_REG_IRQEN_ALSEVERYSAMPLE);

   // measurement rate for auto
  Wire.write(SI1145_REG_MEASRATE0);
  Wire.write(0xFF);

  // auto run
  Wire.write(SI1145_REG_COMMAND);
  Wire.write(SI1145_PSALS_AUTO);

  Serial.println("OK!");
}

void loop() {
  Serial.println("===================");
  Serial.println("========UV index===========");
  Wire.write(0x2C);
  while (Wire.available()) { // peripheral may send less than requested
    Serial.print(Wire.read()); // receive a byte as character

  }

  Serial.println("========Visible & IR light===========");
  Wire.write(0x22);
  while (Wire.available()) { // peripheral may send less than requested
    char c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }

  Serial.println("========Infrared light level===========");
  Wire.write(0x24);
  while (Wire.available()) { // peripheral may send less than requested
    char c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }
   
  //Serial.print("Vis: "); Serial.println(uv.readVisible());
  //Serial.print("IR: "); Serial.println(uv.readIR());
  
 
  //float UVindex = uv.readUV();
  /// the index is multiplied by 100 so to get the
  /// integer index, divide by 100!
  //UVindex /= 100.0;  
  //Serial.print("UV: ");  Serial.println(UVindex);

  delay(1000);
}
