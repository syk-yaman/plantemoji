
# Plantemoji: an IoT plant monitor made in CASA, UCL

A simple device with a web infrastructure to monitor plants and explain their living state in emojis. Basically, plants will be able to talk with you using emojis!

![System components overview](/Docs/Plantemoji-main-photo.jpg)

## Overview

This mind map explains how the system is composed, down from reading sensors and understanding their values, and later sending them up to the cloud for further analysis and visualisation.

![System components overview](/Docs/System-mind-map.jpg)

## Buidling

### Used components

Hardware:
 - Arduino
 - Huzzah
 - Huzzah-DHT22 CASA shield (https://github.com/ucl-casa-ce/casa0014/blob/main/plantMonitor/README.md#multiple-physical-prototypes)
 - DH22
 - HW-390
 - Nails
 - Screen
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
 
