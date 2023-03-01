/*****************************************************************************
* This file is specific profile for the chosen plant:
* - LOWER_MOISTURE_LEVEL depends on a capacitive soil sensor (HW-390) 
*   |___ the maximum value is 550 --> dry sitiuation
*   |___ the minmum value is 120 --> sensor in pure water
* - LOWER_HUMIDITY_LEVEL depends on temperature and humidity sensor (DHT22)
*   |___ the maximum value is 100 --> highest humidity
*   |___ the minimum value is 0 --> lowest humidity
******************************************************************************/

#define PLANT_NAME "Rubber Plant"
#define LOWER_MOISTURE_LEVEL 400
#define LOWER_HUMIDITY_LEVEL 40
