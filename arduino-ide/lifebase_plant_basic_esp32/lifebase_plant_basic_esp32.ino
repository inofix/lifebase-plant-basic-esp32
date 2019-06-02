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
#define SUBJECT_SERVICE_UUID "540b47b8-e337-46ca-9690-cdd6d309e7b1"
#define SUBJECT_NAME_UUID "a62b400e-cef0-474e-a14a-e6f5ee43e0b2"
#define SUBJECT_UUID_UUID "abc4bca0-ea7d-4ea6-86d7-11e456ae6ed0"

// system constants per system/setup
/// #change# These UUIDs should differ from setup to setup
#define SUBJECT_NAME "Most Basic Example Setup"
#define SUBJECT_UUID "e9979b5f-c2c7-45f6-8377-7c94e0b1a7e4"

/// measurements/action - #change# uncoment service UUIDs as needed
///// light service configuration
#define LIGHT_SERVICE_UUID "bb3a6903-bb8a-4b79-be3c-abfd105cbb55"
#if defined LIGHT_SERVICE_UUID
#define LIGHT_SUN_UUID "e835ef9e-e124-4a78-82cc-89bb863835f1"
#define LIGHT_SHADE_UUID "b685c088-3e82-401f-8a51-030d3de0a5d5"
///// light sensor constants
#if defined LIGHT_SUN_UUID
#define LIGHTSUNPIN 34
#endif
#if defined LIGHT_SHADE_UUID
#define LIGHTSHADEPIN 35
#endif
#endif

//// air service configuration
#define AIR_SERVICE_UUID "10fc8ce8-f731-4c22-aba5-e698482fe779"
#if defined AIR_SERVICE_UUID
#define AIR_TEMPERATURE_UUID "1b1090d2-8686-4557-ae77-b97ee0c507f9"
#define AIR_HUMIDITY_UUID "b34ec97e-be28-4328-8ca5-eff8107a181d"
///// air sensor includes
#include <DHT_U.h>
///// air sensor constants
#define DHTPIN 13
#define DHTTYPE DHT22
DHT_Unified dht(DHTPIN, DHTTYPE);
#endif


//// water service configuration
#define WATER_SERVICE_UUID "dad92bf8-4895-4498-9449-9ab55ee4c30c"
#if defined WATER_SERVICE_UUID
#define WATER_CACHEPOT_LEVEL_UUID "394c7715-2635-4b20-9f85-d009749817dc"
#define WATERPIN 33
#endif

#define SOIL_SERVICE_UUID "596b6bb6-cc61-4451-bd26-4aa5a29a58e5"
#if defined SOIL_SERVICE_UUID
#define SOIL_MOISTURE_UUID "54bb543c-5c7b-4275-a061-e6fec9b74914"
#define SOILMONOPIN 25
//#define SOILDUALAPIN 26
//#define SOILDUALDPIN 27
#endif

// BLE includes
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

BLEServer* ble_server = NULL;
BLECharacteristic* subject_uuid_characteristic = NULL;
BLECharacteristic* subject_name_characteristic = NULL;
BLECharacteristic* light_sun_characteristic = NULL;
BLECharacteristic* light_shade_characteristic = NULL;
BLECharacteristic* air_temperature_characteristic = NULL;
BLECharacteristic* air_humidity_characteristic = NULL;
BLECharacteristic* water_cachepot_level_characteristic = NULL;
BLECharacteristic* soil_moisture_characteristic = NULL;
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
    ble_advertising->setMinPreferred(0x0);
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
    get_dht_info();

#if defined LIGHT_SERVICE_UUID
    get_light_info();
#endif
    get_soil_info();
    get_cachepot_info();

    // The DHT does not deliver new results faster than every 2s
    delay(2000);
}
