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

// system constants
#define LB_TAG "LifeBaseMeter"

// subject service
#define SUBJECT_SERVICE_UUID "54000000-e337-46ca-9690-cdd6d309e7b1"
#define SUBJECT_NAME_UUID "54000001-e337-46ca-9690-cdd6d309e7b1"
#define SUBJECT_UUID_UUID "54000002-e337-46ca-9690-cdd6d309e7b1"

// system constants per system/setup
/// #change# These UUIDs should differ from setup to setup
#define SUBJECT_NAME "Most Basic Example Setup"
#define SUBJECT_UUID "e9979b5f-c2c7-45f6-8377-7c94e0b1a7e4"

/// measurements/action - #change# uncoment service UUIDs as needed
///// light service configuration
#define LIGHT_SERVICE_UUID "54010000-e337-46ca-9690-cdd6d309e7b1"
#if defined LIGHT_SERVICE_UUID
#define LIGHT_EXPOSURE_UUID "54010001-e337-46ca-9690-cdd6d309e7b1"
//#define LIGHT_SUN_UUID "54010002-e337-46ca-9690-cdd6d309e7b1"
//#define LIGHT_SHADE_UUID "54010003-e337-46ca-9690-cdd6d309e7b1"
///// light sensor constants
#if defined LIGHT_EXPOSURE_UUID
#define LIGHTSUNPIN 34
#define LIGHTSHADEPIN 35
#endif
#endif

//// air service configuration
#define AIR_SERVICE_UUID "54020000-e337-46ca-9690-cdd6d309e7b1"
#if defined AIR_SERVICE_UUID
#define AIR_TEMPERATURE_UUID "2a6e"
#define AIR_HUMIDITY_UUID "2a6f"
///// air sensor includes
#include <DHT_U.h>
///// air sensor constants
#define DHTPIN 13
#define DHTTYPE DHT22
DHT_Unified dht(DHTPIN, DHTTYPE);
#endif

//// water service configuration
#define WATER_SERVICE_UUID "54030000-e337-46ca-9690-cdd6d309e7b1"
#if defined WATER_SERVICE_UUID
#define WATER_CACHEPOT_LEVEL_UUID "54030001-e337-46ca-9690-cdd6d309e7b1"
#define WATERANALOGLEVELPIN 33
#define WATER_MIN_LEVEL_UUID "42b6e190-ddc5-4899-bd2f-fd17e6f72132"
#define WATERMMINLEVEL 2
#define WATER_MAX_LEVEL_UUID "0f9b4d97-a2ba-4613-9b65-a7a2823b2afd"
#define WATERMMAXLEVEL 4
#define WATER_PUMP_UUID "52afd454-d054-42db-a106-e129052359cd"
#define WATERPUMP 15
#endif

#define SOIL_SERVICE_UUID "54040000-e337-46ca-9690-cdd6d309e7b1"
#if defined SOIL_SERVICE_UUID
#define SOIL_MOISTURE_UUID "54040001-e337-46ca-9690-cdd6d309e7b1"
#define SOILMONOPIN 32
//#define SOILDUALAPIN 26
//#define SOILDUALDPIN 27
#endif

// BLE includes
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <BLE2904.h>

BLEServer* ble_server = NULL;
BLECharacteristic* subject_uuid_characteristic = NULL;
BLECharacteristic* subject_name_characteristic = NULL;
#if defined LIGHT_EXPOSURE_UUID
BLECharacteristic* light_exposure_characteristic = NULL;
#endif
#if defined AIR_SERVICE_UUID
BLECharacteristic* air_temperature_characteristic = NULL;
BLECharacteristic* air_humidity_characteristic = NULL;
#endif
#if defined WATER_SERVICE_UUID
BLECharacteristic* water_cachepot_level_characteristic = NULL;
#endif
#if defined SOIL_SERVICE_UUID
BLECharacteristic* soil_moisture_characteristic = NULL;
#endif
bool device_connected = false;
bool old_device_connected = false;
uint32_t value = 0;

//#define

static void init_sensors() {

    Serial.print("Initializing the sensors.. ");
    initArduino();
#if defined AIR_SERVICE_UUID
    dht.begin();
#endif
    Serial.println("done.");
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

    // Improve the range https://community.openmqttgateway.com/t/esp32-ble-range/249/10
    esp_err_t errRc=esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P9);
    // without the following the range was only slightly better on our device (DevKit v1)
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_SCAN, ESP_PWR_LVL_P9);
//    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_NUM, ESP_PWR_LVL_P9);

    ble_server = BLEDevice::createServer();
    ble_server->setCallbacks(new LBMServerCallbacks());

    BLEService *subject_service = ble_server->createService(SUBJECT_SERVICE_UUID);
    subject_uuid_characteristic = subject_service->createCharacteristic(
            SUBJECT_UUID_UUID, BLECharacteristic::PROPERTY_READ
    );
    subject_name_characteristic = subject_service->createCharacteristic(
            SUBJECT_NAME_UUID, BLECharacteristic::PROPERTY_READ
    );
#if defined LIGHT_SERVICE_UUID
    init_ble_light(ble_server);
#endif
#if defined AIR_SERVICE_UUID
    init_ble_air(ble_server);
#endif
#if defined WATER_SERVICE_UUID
    init_ble_water(ble_server);
#endif
#if defined SOIL_SERVICE_UUID
    init_ble_soil(ble_server);
#endif

//    subject_uuid_characteristic->addDescriptor(new BLE2902());
    subject_uuid_characteristic->setValue(SUBJECT_UUID);
    subject_name_characteristic->setValue(SUBJECT_NAME);
    subject_service->start();
    BLEAdvertising *ble_advertising = BLEDevice::getAdvertising();
    ble_advertising->addServiceUUID(SUBJECT_SERVICE_UUID);
    ble_advertising->setScanResponse(true);
//    ble_advertising->setMinPreferred(0x0);
//    ble_advertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    ble_advertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
}

static void set_ble_characteristic(BLECharacteristic* characteristic, std::string value) {
    // notify changed value
    if (device_connected) {
        characteristic->setValue(value);
        characteristic->notify();
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

#if defined LIGHT_SERVICE_UUID
    get_light_info();
#endif
#if defined AIR_SERVICE_UUID
    get_dht_info();
#endif
#if defined SOIL_SERVICE_UUID
    get_soil_info();
#endif
#if defined WATER_SERVICE_UUID
    get_water_info();
#endif

    // The DHT does not deliver new results faster than every 2s
    delay(2000);
}
