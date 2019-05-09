# lifebase-plant-basic-esp32

## Overview

This project is a first implementation of an environement meter
that can be used to monitor plants.

It consists of just a
  * DHT-22 - humidity and temperature sensor
  * Photoresistor - to measure the light exposure
  * Some capacitive soil moisture meter

## Installation

This project is based on espressif's[1] wunderful ESP32[2].

To setup a working environment, you need the IDF[3] (documentation
for the IDF[5]) and the toolchain[4] (documentation for the
crosstoolchain[6]). The setup of those components is very nicely
documented and quite straightforward.

As we use libraries from the arduino[7] project, you have to
first add the arduino-esp32[8] sources and then add those
libraries all as components to a special subdirectory as follows.
These are the third party libraries from Adafruit[9] (kudos guys!):

  * Unified Sensor Library[10] (e.g. latest stable)
  * DHT Sensor Library[11] (e.g. latest stable)

Create a directory on top of the projects dir for the components
(see [12]) to be put in.

    you@$ mkdir


The arduino-esp32[8] can just be cloned to










--
 [1] https://www.espressif.com/
 [2] https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/index.html
 [3] https://github.com/espressif/esp-idf.git
 [4] https://github.com/espressif/crosstool-NG.git
 [5] https://docs.espressif.com/projects/esp-idf/en/latest/
 [6] https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#setup-toolchain
 [7] https://www.arduino.cc/
 [8] https://github.com/espressif/arduino-esp32.git
 [9] http://adafruit.com/
 [10] https://github.com/adafruit/Adafruit\_Sensor
 [11] https://github.com/adafruit/DHT-sensor-library
 [12] https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
