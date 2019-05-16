/* Life Base's Most Basic Setup Meter

   For copyright and/or -left, warranty, terms of use, and such information,
   please have a look at the LICENSE file in the topmost directory...
*/

// System includes
#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
//#include "esp_log.h"
//#include "nvs_flash.h"
//#include "sdkconfig.h"

// BLE includes
//#include "bt.h"
//#include "bta_api.h"
//#include "esp_gap_ble_api.h"
//#include "esp_gatts_api.h"
//#include "esp_bt_defs.h"
//#include "esp_bt_main.h"

// Sensor includes
#include "DHT_U.h"

// Sensor constants
#define DHTPIN 13
#define DHTTYPE DHT22

DHT_Unified dht(DHTPIN, DHTTYPE);

#define LIGHTPIN 34

#define SOILMONOPIN 25

#define SOILDUALAPIN 26
#define SOILDUALDPIN 27

#define WATERPIN 33

extern "C" {
   void app_main();
}

void get_board_info() {

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("We are running on an ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}

void init_sensors() {

    initArduino();
    dht.begin();
}

void get_dht_info() {

    sensor_t sensor;
    dht.temperature().getSensor(&sensor);
    printf("Humidity/temperature sensor is %s.\n", sensor.name);
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
        printf("Error reading temperature!\n");
    }
    else {
        printf("Current temperature is %f°C.\n", event.temperature);
    }
    dht.humidity().getSensor(&sensor);
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
        printf("Error reading humidity!\n");
    }
    else {
        printf("Current humidity is %f%%.\n", event.relative_humidity);
    }
}

void get_sun_info() {

    printf("Current light exposure is %d.\n", analogRead(LIGHTPIN));
    printf("Current soil moisture reported from the 'mono' is %d.\n", analogRead(SOILMONOPIN));
    printf("Current soil moisture reported from the 'dual' is %d, %d.\n", analogRead(SOILDUALAPIN), digitalRead(SOILDUALDPIN));
}

//void get_shade_info() {


//}

void get_cachepot_info() {

    printf("Current water level reported is %d.\n", analogRead(WATERPIN));
}

void app_main() {

    init_sensors();

    while (true) {
        get_board_info();
        get_dht_info();

        get_sun_info();
        get_cachepot_info();

//        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
