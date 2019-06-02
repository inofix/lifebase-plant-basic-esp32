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

#if defined WATER_SERVICE_UUID

static void init_ble_water(BLEServer* ble_server) {

    BLEService *water_service = ble_server->createService(WATER_SERVICE_UUID);
    water_cachepot_level_characteristic = water_service->createCharacteristic(
            WATER_CACHEPOT_LEVEL_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    water_service->start();
}

//TODO: it is probably more a problem of the device than the view...
//TODO:
static void get_cachepot_info() {

    Serial.print("Current water level reported is ");
    int water_level = analogRead(WATERPIN);
    char water_level_string[4];
    dtostrf(water_level, 4, 0, water_level_string);
    Serial.print(water_level_string);
    Serial.println(".");
    set_ble_characteristic(water_cachepot_level_characteristic, water_level_string);
}

#endif
