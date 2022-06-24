/**
 * This firmware will connect to an oled screen and mifare tag scanner. Once a tag is scanned, 
 * it's id will be send to an API using Wi-Fi. The response of the API will be printed
 * on the oled screen.
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs_flash.h"
#include "u8g2.h"
#include "rc522.h"

#include "oled.h"
#include "wifi.h"
#include "http.h"
#include "api.h"

#define SHOW_DURATION CONFIG_SHOW_DURATION

static const char *TAG = "main";

void onScannedTag(uint8_t* id)
{
    // 1. Notify the user that a tag was scanned
    char *text1[2] = {"Informatie", "laden..."};
    oled_draw_centered_text(text1, 2);

    // 2. Convert the tag into a string
    char id_str[15] = {0};
    sprintf(id_str, "%X:%X:%X:%X:%X", id[0], id[1], id[2], id[3], id[4]);
    ESP_LOGI(TAG, "Scanned Tag: %s", id_str);

    // 3. Get the tags information from the API
    char name[16] = {0};
    double price = 0.0;
    esp_err_t api_result = api_request_name(id_str, name, &price);

    // 4. Parse the API response
    if (api_result == ESP_OK)
    {
        ESP_LOGI(TAG, "Item \"%s\" is %2.2f", name, price);
        oled_draw_product(name, price);
    }
    else
    {
        ESP_LOGW(
            TAG,
            "Could not fetch data: api_result = %s (%d)",
            esp_err_to_name(api_result),
            api_result
        );

        switch (api_result) {
            case ESP_ERR_HTTP_CONNECT: {
                char *texterr[1] = {"Server offline"};
                oled_draw_centered_text(texterr, 1);
                break;
            };
            case ESP_ERR_HTTP_MAX_REDIRECT: {
                // For some reason esp_http_client_perform returns ESP_ERR_HTTP_MAX_REDIRECT when status code 422 is received
                char *texterr[2] = {"Product niet", "gevonden"};
                oled_draw_centered_text(texterr, 2);
                break;
            }
            default: {
                char *texterr[2] = {"Kon gegevens", "niet ophalen"};
                oled_draw_centered_text(texterr, 2);
                break;
            };
        }        
    }
    // 5. Wait for a bit so the user can see the message
    vTaskDelay(SHOW_DURATION / portTICK_PERIOD_MS);

    // 6. Tell the user that a new card can be scanned 
    char *text2[1] = {"Presenteer kaart"};
    oled_draw_centered_text(text2, 1);
}

/**
 * @brief Start the oled screen and write a message on it. 
 */
void start_oled()
{
    oled_init();

    char *text[1] = {"Opstarten..."};
    oled_draw_centered_text(text, 1);
}

/**
 * @brief Make a wi-fi connection to the configured AP.
 * 
 * @return esp_err_t 
 */
esp_err_t start_wifi()
{
    esp_err_t wifi_result = wifi_init_sta();

    if (wifi_result != ESP_OK)
    {
        char *errstr[2] = {"Geen Wi-Fi", "Verbinding!"};
        oled_draw_centered_text(errstr, 2);

        ESP_LOGW(
            TAG,
            "Early app_main exit: wifi_result = %s (%d)",
            esp_err_to_name(wifi_result),
            wifi_result
        );
    }

    return wifi_result;
}

/**
 * @brief Start the tag reader.
 * The tag reader communicates over SPI.
 * The following component is used: https://github.com/abobija/esp-idf-rc522
 * 
 * @return esp_err_t
 */
esp_err_t start_tag_reader()
{
    const rc522_start_args_t nfc_args = {
        .miso_io = 19,
        .mosi_io = 23,
        .sck_io = 18,
        .sda_io = 5,
        .callback = onScannedTag
    };

    esp_err_t tag_result = rc522_start(nfc_args);

    if (tag_result != ESP_OK)
    {
        char *errstr[2] = {"Tag scanner", "niet bereikbaar"};
        oled_draw_centered_text(errstr, 2);
        
        ESP_LOGW(
            TAG,
            "Early app_main exit: tag_result = %s (%d)",
            esp_err_to_name(tag_result),
            tag_result
        );
    }

    return tag_result;
}

/**
 * @brief Application entrypoint
 * Initializes all components
 */
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

    // First initialize the screen, so the user knows what is happening
    start_oled();

    if (start_wifi() != ESP_OK)
        return;

    if (start_tag_reader() != ESP_OK)
        return;

    // Load mac-address for api requests
    api_get_address();

    // Give the user instructions
    char *text1[1] = {"Presenteer kaart"};
    oled_draw_centered_text(text1, 1);

    ESP_LOGI(TAG, "End app_main");
}
