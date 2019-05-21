/* Life Base's Most Basic Setup Meter

   For copyright and/or -left, warranty, terms of use, and such information,
   please have a look at the LICENSE file in the topmost directory...
*/

// System includes
#include <stdio.h>
//#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "freertos/event_groups.h"
//#include "esp_system.h"
//#include "esp_spi_flash.h"
#include "esp_log.h"
//#include "nvs_flash.h"
//#include "sdkconfig.h"

// Sensor includes
#include "DHT_U.h"

// BLE includes
#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"

// System constants
#define LB_TAG "LifeBaseMeter"

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

extern "C" {
   void app_main();
}

static void get_board_info() {

    /* Print chip information */
/*    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    ESP_LOGI(LB_TAG, "We are running on an ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    ESP_LOGI(LB_TAG, "silicon revision %d, ", chip_info.revision);

    ESP_LOGI(LB_TAG, "%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
*/
}

static void init_sensors() {

    initArduino();
    dht.begin();
}

static void get_dht_info() {

    sensor_t sensor;
    dht.temperature().getSensor(&sensor);
    ESP_LOGI(LB_TAG, "Humidity/temperature sensor is %s.\n", sensor.name);
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
        ESP_LOGI(LB_TAG, "Error reading temperature!\n");
    }
    else {
        ESP_LOGI(LB_TAG, "Current temperature is %f°C.\n", event.temperature);
    }
    dht.humidity().getSensor(&sensor);
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
        ESP_LOGI(LB_TAG, "Error reading humidity!\n");
    }
    else {
        ESP_LOGI(LB_TAG, "Current humidity is %f%%.\n", event.relative_humidity);
    }
}

static void get_light_info() {

    ESP_LOGI(LB_TAG, "Current light sun exposure is %d.\n", analogRead(LIGHTSUNPIN));
    ESP_LOGI(LB_TAG, "Current light shade exposure is %d.\n", analogRead(LIGHTSHADEPIN));
}

static void get_soil_info() {

    ESP_LOGI(LB_TAG, "Current soil moisture reported from the 'mono' is %d.\n", analogRead(SOILMONOPIN));
    ESP_LOGI(LB_TAG, "Current soil moisture reported from the 'dual' is %d, %d.\n", analogRead(SOILDUALAPIN), digitalRead(SOILDUALDPIN));
}

//void get_shade_info() {


//}

static void get_cachepot_info() {

    ESP_LOGI(LB_TAG, "Current water level reported is %d.\n", analogRead(WATERPIN));
}

#define SERVICE_UUID        "a9a05640-5655-4db4-a4be-0df1adfcb932"
#define CHARACTERISTIC_UUID "b5ab272d-66d4-40ab-9cc3-bd7dabcf3c21"

static void init_ble() {
    BLEDevice::init("LifeBase Plant Monitor");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
    pCharacteristic->setValue("Hi ..");
    pService->start();
    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
}

void app_main() {

    init_sensors();
     init_ble();

    get_board_info();

    while (true) {
        get_dht_info();

        get_light_info();
        get_soil_info();
        get_cachepot_info();

//        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
