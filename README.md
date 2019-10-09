# lifebase-plant-basic-esp32

## Overview

This project is a first implementation of an environement meter
that can be used to monitor soil based plants.

The hardware is based on espressif's[1] wonderful ESP32[2] and
we use the development board from DOIT[3] (DOIT ESP32 DEVKIT V1).

The current setup consists of just the following sensors

  * DHT22 - humidity and temperature sensor
  * 2 photoresistors - to measure the light exposure
  * some capacitive soil moisture meter
  * some float switch to measure the water level and actuators
  * a pump

## Installation

### Arduino IDE

As my toolchain with the ESP IDF stopped working at
a certain point. I tried out the Arduino IDE which
immediately just did work.
As I am not very happy with this solution, I will
try again with the ESP IDF later, but for now
we recommend using this solution:

  * Install the Arduino IDE
  * Add the board
   * Go to: "Tools", "Board"
   * Choose:
    * "DOIT ESP32 DEVKIT V1"
  * Add the libraries
   * Go to: "Sketch", "Include Library", "Manage Libraries"
   * Choose:
    * "Adafruit Unified Sensor"
    * "DHT sensor library"
    * "ESP32"
    * "ESP32 BLE Arduino"

The development is in the subfolder calles `arduino-ide`, just
open the main .ino file in your IDE.

### ESP IDF

*Warning* This section is currently not working. See the
Arduino IDE section above..

To setup a working environment, you need the IDF[4] (documentation
for the IDF[5]) and the toolchain[6] (documentation for the
crosstoolchain[7]) - and obviously, the ESP32 hardware itself.
The setup of those components is very nicely
documented and quite straightforward (nice work guys!).

As we use libraries from the arduino[8] project, you have to
first add the arduino-esp32[9] sources and then add those
libraries all as components to a special subdirectory as follows.
These are the third party libraries from Adafruit[10] (kudos guys!):

  * Unified Sensor Library[11] (e.g. latest stable)
  * DHT Sensor Library[12] (e.g. latest stable)

Create a directory called `components` on top of the projects
dir for the components (see [13]) to be put in - you may also
just softlink to the library sources...

The detailed instructions for the arduino-esp32 can be found
under [14].

The Adafruit[9] libraries can be cloned to the same folder too.
Afterwards just create an empty file in the top folder called
`component.mk` and create a directory called `include` where
you link all the header files you need into (\*.h) as the
esp-idf will search them there.

## Bugs

Any bug report or feature request is warmly welcome via
the issue tracker here.

At the moment we have not quite decided yet how to handle
all the values recieved from the sensors - e.g. while for
light there are standardized units that we will probably
use (i.e. Lux), we have absolutely no idea how to handle
soil moisture. As a pragmatic approach we will just use
percentage for everything as a first approach and then
later replace it with the definite unit. Let's say, if
the BLE unit characteristic is not set, it is assumed
to be a temporarily percentage value.

---
    [1] https://www.espressif.com/
    [2] https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/index.html
    [3] http://doit.am
    [4] https://github.com/espressif/esp-idf.git
    [5] https://docs.espressif.com/projects/esp-idf/en/latest/
    [6] https://github.com/espressif/crosstool-NG.git
    [7] https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#setup-toolchain
    [8] https://www.arduino.cc/
    [9] https://github.com/espressif/arduino-esp32.git
    [10] http://adafruit.com/
    [11] https://github.com/adafruit/Adafruit\_Sensor
    [12] https://github.com/adafruit/DHT-sensor-library
    [13] https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
    [14] https://github.com/espressif/arduino-esp32/blob/master/docs/esp-idf_component.md
