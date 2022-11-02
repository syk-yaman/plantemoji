
# Plantemoji: an IoT plant monitor made in CASA, UCL

A simple device with a web infrastructure to monitor plants and explain their living state in emojis. Basically, plants will be able to talk with you using emojis!

![System components overview](/Docs/Plantemoji-main-photo.jpg)

## Overview

This mind map explains how the system is composed, down from reading sensors and understanding their values, and later sending them up to the cloud for further analysis and visualisation.

![System components overview](/Docs/System-mind-map.jpg)

## Buidling

### Used components

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
 
### Hardware schematic

![Hardware schematic](/Docs/schematic.png)

## Results discusion

## Future work

## Resources and references

## Lessons learned
 - Understanding the data from a simple sensor in the real world is not easy due to the effect of environment parameters and setup conditions.
 - Print a 3D case and try to be gentle with screens!
 - Visualising the data can make us understand the surrounding environment better.
 - If a serial connection is needed between two devices with different power sources, their GND should be connected together. Of course, they also must have the same baud rate.
 
