/*
    Life Base's Most Basic Setup Meter

    The goal is to have a first version of a working prototype for our most
    basic plant monitoring device.

    This work was inspired by examples from the arduino-ide and esp-idf, resp.
    arduino-esp:

    - https://github.com/espressif/arduino-esp32/blob/master/libraries/BLE/examples/
    - https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp

    For copyright and/or -left, warranty, terms of use, and such information,
    please have a look at the LICENSE file in the topmost directory...
*/

#if defined SOIL_SERVICE_UUID

// The sensor used here is a 'Capacitive Soil Moisture Sensor v1.2' from
// diymore.cc. We protected it against the permanent exposure in a
// potentially wet and unfriendly environment with lacquer (electronics
// and PCB edges..).
// In the reference implementation with a very loose and humus-rich soil
// we found the following values:
//   - in water: 31488 (100%)
//   - after watering: 47133 (38%)
//   - 'quite dry' soil: 52944 (14%)
//   - in complete isolation: 56368 (0%)
//TODO: verify or specify for differnt soil types..

#define SOIL_MOISTURE_ABSOLUTE_MIN 56399
#define SOIL_MOISTURE_ABSOLUTE_MAX 31488

// MIN/MAX moisture depend on the plant and soil and we might even want
// to set it dynamically on the device and store it.
//TODO: store it
//TODO: authentication needed
//..
// default minimum of soil moisture (or when to water / warn / etc.)
#define SOIL_MOISTURE_MIN 15
// default maximum of soil moisture (or when to open valve / warn / etc.)
#define SOIL_MOISTURE_MAX 86

static void init_soil() {

    pinMode(SOILMOISTUREPIN, INPUT);
}

static void init_ble_soil(BLEServer* ble_server) {

    BLEService *soil_service = ble_server->createService(SOIL_SERVICE_UUID);
    soil_moisture_characteristic = soil_service->createCharacteristic(
            SOIL_MOISTURE_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    soil_moisture_characteristic->addDescriptor(new BLE2902());
    soil_service->start();
}

static int read_soil_moisture_percentage(int PIN) {

    int i = (100 - (analogRead(SOILMOISTUREPIN) - SOIL_MOISTURE_ABSOLUTE_MAX) *
            100 / (SOIL_MOISTURE_ABSOLUTE_MIN - SOIL_MOISTURE_ABSOLUTE_MAX));

    if (i < SOIL_MOISTURE_MIN) {
        if (i < 0) {
            i = 0;
        }
        is_too_dry = true;
        is_too_wet = false;
    } else if (i < SOIL_MOISTURE_MAX) {
        is_too_wet = true;
        is_too_dry = false;
    } else {
        is_too_wet = false;
        is_too_dry = false;
    }
    return i;
}

static void get_soil_info() {

    int soil_moisture = read_soil_moisture_percentage(SOILMOISTUREPIN);
    char soil_moisture_chars[3];
    dtostrf(soil_moisture, 3, 0, soil_moisture_chars);
    Serial.print("Current soil moisture reported is ");
    Serial.print(soil_moisture);
    Serial.println("%.");
    set_ble_characteristic(soil_moisture_characteristic, soil_moisture_chars);
}

#endif
