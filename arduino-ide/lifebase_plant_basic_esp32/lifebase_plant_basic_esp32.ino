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

// Sensor includes
#include <DHT_U.h>

// BLE includes
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// Sensor constants
#define DHTPIN 13
#define DHTTYPE DHT22

DHT_Unified dht(DHTPIN, DHTTYPE);

#define LIGHTSUNPIN 34
#define LIGHTSHADEPIN 35

#define SOILMONOPIN 25

#define SOILDUALAPIN 26
#define SOILDUALDPIN 27

#define WATERPIN 33

// BLE constants
#define LB_TAG "LifeBaseMeter"
#define SUBJECT_SERVICE_UUID "540b47b8-e337-46ca-9690-cdd6d309e7b1"
#define SUBJECT_NAME_CHARACTERISTIC_UUID "a62b400e-cef0-474e-a14a-e6f5ee43e0b2"
#define SUBJECT_UUID_CHARACTERISTIC_UUID "abc4bca0-ea7d-4ea6-86d7-11e456ae6ed0"

///startTODO: this should be generated automatically later..
#define SUBJECT_NAME "8e419fa5-375e-4b59-9dce-9b27a0e3d0e1"
#define SUBJECT_UUID "e9979b5f-c2c7-45f6-8377-7c94e0b1a7e4"
///endTODO

/// Measurements
#define WATER_CACHEPOT_LEVEL_UUID "e835ef9e-e124-4a78-82cc-89bb863835f1"

BLEServer* ble_server = NULL;
BLECharacteristic* subject_uuid_characteristic = NULL;
BLECharacteristic* subject_name_characteristic = NULL;
BLECharacteristic* light_sun_characteristic = NULL;
BLECharacteristic* light_shade_characteristic = NULL;
BLECharacteristic* air_temperature_characteristic = NULL;
BLECharacteristic* air_humidity_characteristic = NULL;
BLECharacteristic* soil_moisture_characteristic = NULL;
BLECharacteristic* water_cachepot_level_characteristic = NULL;
bool device_connected = false;
bool old_device_connected = false;
uint32_t value = 0;

//#define

static void init_sensors() {

    Serial.print("Initializing the sensors.. ");
    initArduino();
    dht.begin();
    Serial.println("done.");
}

static void get_dht_info() {

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
    }
}

static void get_light_info() {

    Serial.print("Current light sun exposure is ");
    Serial.print(analogRead(LIGHTSUNPIN));
    Serial.println(".");
    Serial.print("Current light shade exposure is ");
    Serial.print(analogRead(LIGHTSHADEPIN));
    Serial.println(".");
}

static void get_soil_info() {

    Serial.print("Current soil moisture reported from the 'mono' is ");
    Serial.print(analogRead(SOILMONOPIN));
    Serial.println(".");
    Serial.print("Current soil moisture reported from the 'dual' is ");
    Serial.print(analogRead(SOILDUALAPIN));
    Serial.print(", ");
    Serial.print(digitalRead(SOILDUALDPIN));
    Serial.println(".");
}

//void get_shade_info() {


//}

static void get_cachepot_info() {

    Serial.print("Current water level reported is ");
    int water_level = analogRead(WATERPIN);
    char water_level_string[4];
    dtostrf(water_level, 4, 0, water_level_string);
    Serial.print(water_level);
    set_ble_characteristic(water_level_string);
    Serial.println(".");
}

class LBMServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* ble_server) {
      device_connected = true;
    };

    void onDisconnect(BLEServer* ble_server) {
      device_connected = false;
    }
};

static void init_ble() {

    BLEDevice::init(LB_TAG);

    ble_server = BLEDevice::createServer();
    ble_server->setCallbacks(new LBMServerCallbacks());

    BLEService *ble_service = ble_server->createService(SUBJECT_SERVICE_UUID);
    subject_uuid_characteristic = ble_service->createCharacteristic(
            SUBJECT_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE
    );

    subject_uuid_characteristic->addDescriptor(new BLE2902());
    subject_uuid_characteristic->setValue(SUBJECT_NAME);
    ble_service->start();
    BLEAdvertising *ble_advertising = BLEDevice::getAdvertising();
    ble_advertising->addServiceUUID(SUBJECT_SERVICE_UUID);
    ble_advertising->setScanResponse(true);
    ble_advertising->setMinPreferred(0x0);
    BLEDevice::startAdvertising();
}

static void set_ble_characteristic(std::string value) {
    // notify changed value
    if (device_connected) {
        subject_uuid_characteristic->setValue(value);
//        subject_uuid_characteristic->setValue("foobar");
        subject_uuid_characteristic->notify();
        delay(3); // Based on BLEnotify: bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
    }
    // disconnecting
    if (!device_connected && old_device_connected) {
        delay(500); // Based on BLEnotify: give the bluetooth stack the chance to get things ready
        ble_server->startAdvertising(); // Based on BLEnotify: restart advertising
        old_device_connected = device_connected;
    }
    // connecting
    if (device_connected && !old_device_connected) {
        // do stuff here on connecting
        old_device_connected = device_connected;
    }
}

void setup() {

    Serial.begin(115200);
    init_ble();
    init_sensors();
}

void loop() {
    Serial.println("--");
    get_dht_info();

    get_light_info();
    get_soil_info();
    get_cachepot_info();

    // The DHT does not deliver new results faster than every 2s
    delay(2000);
}
