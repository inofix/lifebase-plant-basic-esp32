/*
    Life Base's Most Basic Setup Meter

    The goal is to have a first version of a working prototype for our most
    basic plant monitoring device.

    This work was inspired by examples from the arduino-ide and esp-idf, resp.
    arduino-esp:

    - https://github.com/espressif/arduino-esp32/blob/master/libraries/BLE/examples/BLE_server/BLE_server.ino
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

// System constants
#define LB_TAG "LifeBaseMeter"
//#define LB_NAME "Basic Plant Life Base Meter"

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
#define SERVICE_UUID "540b47b8-e337-46ca-9690-cdd6d309e7b1"
#define CHARACTERISTIC_UUID "e9979b5f-c2c7-45f6-8377-7c94e0b1a7e4"

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
    Serial.print(analogRead(WATERPIN));
    Serial.println(".");
}

static void init_ble() {

    BLEDevice::init(LB_TAG);
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE);

    pCharacteristic->setValue("Hell of a World");
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
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
