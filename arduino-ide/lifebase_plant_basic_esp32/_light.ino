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

#if defined LIGHT_SERVICE_UUID

static void init_ble_light(BLEServer* ble_server) {

    BLEService *light_service = ble_server->createService(LIGHT_SERVICE_UUID);
    light_sun_characteristic = light_service->createCharacteristic(
            LIGHT_SUN_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE
    );
    light_shade_characteristic = light_service->createCharacteristic(
            LIGHT_SHADE_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE
    );
    light_service->start();
}

//TODO: it is probably more a problem of the device than the view...
//TODO:   - we might want to send descrete values instead of two
//TODO:     (or more) sensor results, e.g. dark|shady|low|bright|burning
static void get_light_info() {

    Serial.print("Current light sun exposure is ");
    int light = analogRead(LIGHTSUNPIN);
    char light_string[4];
    dtostrf(light, 4, 0, light_string);
    Serial.print(light_string);
    Serial.println(".");
    set_ble_characteristic(light_sun_characteristic, light_string);
    Serial.print("Current light shade exposure is ");
    light = analogRead(LIGHTSHADEPIN);
    dtostrf(light, 4, 0, light_string);
    Serial.print(light_string);
    Serial.println(".");
    set_ble_characteristic(light_shade_characteristic, light_string);
}

#endif
