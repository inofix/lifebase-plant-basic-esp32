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
    light_exposure_characteristic = light_service->createCharacteristic(
            LIGHT_EXPOSURE_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    light_exposure_characteristic->addDescriptor(new BLE2902());
    BLE2904 *desc0 = new BLE2904();
    desc0->setUnit(0x27AD);
    light_exposure_characteristic->addDescriptor(desc0);
    light_service->start();
}

//TODO: figure out how to display light exposure
//        - let's use a custom defined percentage value for now
static void get_light_info() {

    int light_sun = analogRead(LIGHTSUNPIN);
    int light_shade = analogRead(LIGHTSHADEPIN);
//TODO: figure out dynamically how long addresses are..
    int light_percent = (light_sun + light_shade) * 100 / 8192;
    char light_string[3];
    dtostrf(light_percent, 3, 0, light_string);
    set_ble_characteristic(light_exposure_characteristic, light_string);
    Serial.print("The current light exposure is ");
    Serial.print(light_sun);
    Serial.print("% - (sun: ");
    Serial.print(light_sun);
    Serial.print("; shade: ");
    Serial.print(light_shade);
    Serial.println(").");
}

#endif
