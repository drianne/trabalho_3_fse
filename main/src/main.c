#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include <stdio.h>

#include "../inc/wifi.h"
#include "../inc/http_client.h"
#include "../inc/led.h"
#include "../inc/json.h"

#define IP_STACK_KEY CONFIG_IPSTACK_ACCESS_KEY
#define OPEN_WEATHER_MAP_KEY CONFIG_OPENWEATHERMAPORG_ACCESS_KEY

#define DELAY_REQUEST 300000 // 300000ms = 5min

xSemaphoreHandle conexaoWifiSemaphore = NULL;
xSemaphoreHandle conexaoLedSemaphore = NULL; // task for led control

Infos info;

void FlashLed(void *params){
    while (true){
        if (xSemaphoreTake(conexaoLedSemaphore, portMAX_DELAY)){
            flash_led();
            xSemaphoreGive(conexaoLedSemaphore);
        }
    }
}

void RequestIpStack(void *params){
    while (true){
        if (xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY)){
            char url[110];

            flash_led();
            ESP_LOGI("Stack request", "Requisição Stack");        
               
            sprintf(url,
                    "http://api.ipstack.com/check?access_key=%s&fields=latitude,longitude",
                    IP_STACK_KEY);
            http_request(url);
            xSemaphoreGive(conexaoWifiSemaphore);
            vTaskDelay(DELAY_REQUEST / portTICK_PERIOD_MS);
        }
    }
}


void RequestOpenWeatherMap(void *params){
    while (true){
        if (xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY)){
            char url[150];

            flash_led();
            ESP_LOGI("IWeather Map", "Requisição Weather");
            
            sprintf(url,
                    "http://api.openweathermap.org/data/2.5/weather?lat=%lf&lon=%lf&units=metric&appid=%s",
                    info.latitude,
                    info.longitude,
                    OPEN_WEATHER_MAP_KEY);

            http_request(url);

            printf("\n------------- Dados atuais de Clima e Tempo -------------\n\n");
            printf("\nTemperatura Mínima: %.1lf°C\n", info.temperature_min);
            printf("Temperatura Máxima: %.1lf°C\n", info.temperature_max);
            printf("Temperatura Atual: %.1lf°C\n", info.temperature);
            printf("Humidade: %.1lf%%\n", info.humidity);

            xSemaphoreGive(conexaoWifiSemaphore);
            vTaskDelay(DELAY_REQUEST / portTICK_PERIOD_MS);
        }
        
    }
}


void app_main(void)
{
    conexaoLedSemaphore = xSemaphoreCreateBinary();

    xSemaphoreGive(conexaoLedSemaphore);
    start_led();
    xTaskCreate(&FlashLed, "Liga Led", 1024, NULL, 1, NULL);

    // Init NVS
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    conexaoWifiSemaphore = xSemaphoreCreateBinary();
    
    start_wifi();

    xTaskCreate(&RequestIpStack, "HTTP Request Location", 4096, NULL, 1, NULL);
    xTaskCreate(&RequestOpenWeatherMap, "HTTP Request Weather", 4096, NULL, 1, NULL);
}