
# Plantemoji: an IoT plant monitor made in CASA, UCL

A simple device with a web infrastructure to monitor and control plants environment and explain their living state in emojis. Basically, plants will be able to talk with you using emojis!



![System components overview](/Docs/Device photos/Full device.jpg)

![System components overview](/Docs/Device photos/Pot 3D design.gif)

![System components overview](/Docs/Device photos/Water bucket 3D design.gif)

## Overview

This conceptual diagram explains how the system is composed, down from reading sensors and understanding their values, and later sending them up to the cloud for further analysis and visualisation.

![System components overview](/Docs/System-mind-map.jpg)

The objectives of this plant monitor are:
 - Interpret sensor data for the end user in a human-friendly way.
 - Take into consideration plant living requirements during the monitoring process.
 - Make comparisons between different soil moisture sensors and the same sensor in different conditions.

## Building

### Used components and tools

Hardware:
 - [Arduino UNO](https://store.arduino.cc/products/arduino-uno-rev3)
 - [Adafruit Feather HUZZAH ESP8266](https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/)
 - Huzzah-DHT22 CASA shield ([From here](https://github.com/ucl-casa-ce/casa0014/blob/main/plantMonitor/README.md#multiple-physical-prototypes))
 - [DH22](https://www.adafruit.com/product/385): Humidity and temperature sensor 
 - [HW-390](https://thepihut.com/products/capacitive-soil-moisture-sensor?variant=32137736421438): Capacitive Soil Moisture Sensor 
 - [Waveshare 1.47" LCD Dislay](https://thepihut.com/products/1-47-rounded-spi-lcd-display-module-172x320?variant=42062646509763)

Software:
 - [InfluxDB](https://www.influxdata.com/)
 - [Telegraf](https://www.influxdata.com/time-series-platform/telegraf/)
 - [Grafana](https://grafana.com/)
 - [Docker Compose](https://docs.docker.com/compose/)

All the software applications are packed into one [Docker image](/Server/docker-compose.yml) that you can use easily.

Tools:
 - [Image2Lcd](https://www.waveshare.com/wiki/File:Image2Lcd.7z): converts BMP images to byte arrays stored in MCUs to be displayed on the LCD screen (please follow [these settings](/Docs/Image2LCD%20app%20settings.jpg)).

### Hardware schematic

![Hardware schematic](/PCB/schematic.png)

### Highlights
 - Sensors are connected to Arduino now due to the availability of many ADC ports compared to HUZZAH.
 - The LCD screen driver was ported from Arduino to work on HUZZAH, the latter has more memory and capability to drive screens compared to Arduino.
 - New MQTT topics added: `moistureCapacitive` for the capacitive sensor and `mood` which is derived from sensor data according to plant profile.
 - [Plant profile](/Code/Huzzah/PlantProfile.h): a file containing the suitable living environment parameters for the desired plant. The mood of the plant (happy - sad) is determined based on this profile.

## Results
Grafana dashboard            |  InfluxDB dashboard		 |  MQTT feed
:---------------------------:|:-------------------------:|:-------------------------:
![](/Docs/grafana-result.jpg)|   ![](/Docs/influxdb-result.jpg)|   ![MQTT](/Docs/mqtt-result.jpg)

See [here](/Docs/DAQ/DAQ.md) for more detailed comparisons between sensor types.

## Future work
- Add more plants profiles.
- Let plant profile can be controlled using HTTP.
- Add mobile notifications.
- Do more comparisons with other sensor conditions.
- Add more emojis to express plant mood further.

## Resources and references
- [Capacitive sensors principles](https://makersportal.com/blog/2020/5/26/capacitive-soil-moisture-calibration-with-arduino)
- [Serial communication](https://circuitdigest.com/microcontroller-projects/arduino-spi-communication-tutorial)
- [InfluxDB course](https://university.influxdata.com/courses/influxdb-essentials-tutorial/)

## Lessons learned
 - Understanding the data from a simple sensor in the real world is not easy due to the effect of environment parameters and setup conditions.
 - Remember: Always print a 3D case and try to be gentle with screens!
 - Visualising the data can make us understand the surrounding environment in a better way.
 - If a serial connection is needed between two devices with different power sources, their GND should be connected together. And of course, they also must have the same baud rate.

 

## License


###### The software side is licensed under MIT License

```
MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

###### The hardware side is licensed under CERN License

```
This documentation describes Open Hardware and is licensed under the CERN OHL v. 1.2.

You may redistribute and modify this documentation under the terms of the CERN OHL v.1.2. (http://ohwr.org/cernohl). This documentation is distributed WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING OF MERCHANTABILITY, SATISFACTORY QUALITY AND FITNESS FOR A PARTICULAR PURPOSE. Please see the CERN OHL v.1.2 for applicable conditions
```
