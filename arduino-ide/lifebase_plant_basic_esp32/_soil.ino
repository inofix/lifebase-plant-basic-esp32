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

static void init_ble_soil(BLEServer* ble_server) {

    BLEService *soil_service = ble_server->createService(SOIL_SERVICE_UUID);
    soil_moisture_characteristic = soil_service->createCharacteristic(
            SOIL_MOISTURE_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    soil_moisture_characteristic->addDescriptor(new BLE2902());
    soil_service->start();
}

static void get_soil_info() {

    Serial.print("Current soil moisture reported from the 'mono' is ");
    int soil_moisture = analogRead(SOILMONOPIN);
    char soil_moisture_string[4];
    dtostrf(soil_moisture, 4, 0, soil_moisture_string);
    Serial.print(soil_moisture_string);
    Serial.println(".");
    set_ble_characteristic(soil_moisture_characteristic, soil_moisture_string);
//    Serial.print("Current soil moisture reported from the 'dual' is ");
//    Serial.print(analogRead(SOILDUALAPIN));
//    Serial.print(", ");
//    Serial.print(digitalRead(SOILDUALDPIN));
//    Serial.println(".");
}

#endif
