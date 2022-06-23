#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs_flash.h"

#include "wifi.h"
#include "http.h"
#include "api.h"

static const char *TAG = "entrypoint";

static void http_test_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Begin http_test_task");

    char name[20] = {0};
    double price = 0.0;
    api_request_name("1234", name, &price);

    ESP_LOGI(TAG, "Item \"%s\" is %2.2f", name, price);

    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Start app_main");

    // Initialize the NVS
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    wifi_init_sta();

    api_get_address();
    xTaskCreate(&http_test_task, "http_test_task", 8192, NULL, 5, NULL);

    ESP_LOGI(TAG, "End app_main");
}
