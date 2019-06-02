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

#if defined AIR_SERVICE_UUID

static void init_ble_air(BLEServer* ble_server) {

    BLEService *air_service = ble_server->createService(AIR_SERVICE_UUID);
    air_temperature_characteristic = air_service->createCharacteristic(
            AIR_TEMPERATURE_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    air_humidity_characteristic = air_service->createCharacteristic(
            AIR_HUMIDITY_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    air_service->start();
}


//TODO: it is probably more a problem of the device than the view...
//TODO:   - farenheit vs. celsius? deliver both?
//TODO:   - also send the unit?
//TODO:     - send it via descriptor?
static void get_dht_info() {

    char air_string[4];
    sensor_t sensor;
    dht.temperature().getSensor(&sensor);
    Serial.print("Humidity/temperature sensor is ");
    Serial.print(sensor.name);
    Serial.println(".");

    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
        Serial.println("Error reading temperature!");
    }
    else {
        Serial.print("Current temperature is ");
        Serial.print(event.temperature);
        Serial.println("°C.");
        dtostrf(event.temperature, 4, 0, air_string);
        set_ble_characteristic(air_temperature_characteristic, air_string);
    }
    dht.humidity().getSensor(&sensor);
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
        Serial.println("Error reading humidity!");
    }
    else {
        Serial.print("Current humidity is ");
        Serial.print(event.relative_humidity);
        Serial.println("%");
        dtostrf(event.temperature, 4, 0, air_string);
        set_ble_characteristic(air_humidity_characteristic, air_string);
    }
}

#endif
