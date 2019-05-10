/* Life Base's Most Basic Setup Meter

   For copyright and/or -left, warranty, terms of use, and such information,
   please have a look at the LICENSE file in the topmost directory...
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "DHT_U.h"

#define DHTPIN 13
#define DHTTYPE DHT22

DHT_Unified dht(DHTPIN, DHTTYPE);

#define LIGHTPIN 34

extern "C" {
   void app_main();
}

void app_main() {
    initArduino();

    dht.begin();


    printf("Hello user, this is your setup meter.\n");

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

    printf("Current light exposure is %d\n", analogRead(LIGHTPIN));

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
