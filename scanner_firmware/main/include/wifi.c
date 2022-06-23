/**
 * See: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html#wi-fi-lwip-init-phase
 * And the station_example
 */

#include "wifi.h"

/**
 * @brief The event handler for all wifi events and the GOT_IP event
 * It will try to make a connection until WIFI_RETR is reached
 */
void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        // Wi-Fi has started, try to connect to the AP
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        // Failed to connect, retry if possibles
        if (wifi_retries < WIFI_RETR)
        {
            esp_wifi_connect();
            wifi_retries++;
            ESP_LOGW(WIFI_TAG, "Retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        // Got an IP address, ready to send data over Wi-Fi
        ip_event_got_ip_t *event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(WIFI_TAG, "Got ip: " IPSTR, IP2STR(&event->ip_info.ip));

        wifi_retries = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_sta_init_phase(void) 
{
    ESP_LOGI(WIFI_TAG, "Begin init phase");

    // s1.1: Create an LwIP core and initialize
    ESP_ERROR_CHECK(esp_netif_init());

    // s1.2: Create system event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // s1.3: Setup as station
    esp_netif_create_default_wifi_sta();

    // s1.4: Create wi-fi driver task and initialize it
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_LOGI(WIFI_TAG, "End init phase");
}

void wifi_sta_setup_events(esp_event_handler_instance_t *instance_any_id, esp_event_handler_instance_t *instance_got_ip)
{
    // Setup listener for all WIFI_EVENTs
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            wifi_event_handler,
            NULL,
            instance_any_id
        )
    );

    // Setup listener for IP_EVENT_STA_GOT_IP
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            wifi_event_handler,
            NULL,
            instance_got_ip
        )
    );
}

void wifi_sta_cleanup_events(esp_event_handler_instance_t *instance_any_id, esp_event_handler_instance_t *instance_got_ip)
{
    ESP_ERROR_CHECK(
        esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, *instance_got_ip)
    );

    ESP_ERROR_CHECK(
        esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, *instance_any_id)
    );
}

void wifi_sta_config_phase(void)
{
    ESP_LOGI(WIFI_TAG, "Begin config phase");

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
	        .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    

    ESP_LOGI(WIFI_TAG, "End config phase");
}

/**
 * @brief Wait until the wifi-event is finished
 * 
 * @return esp_err_t Was a connection made?
 */
esp_err_t wifi_sta_wait_connection(void) 
{
    ESP_LOGI(WIFI_TAG, "Begin wait for connection...");

    EventBits_t bits = xEventGroupWaitBits(
        wifi_event_group,
        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdFALSE,    // Don't clear on exist
        pdFALSE,    // Don't wait for all bits
        portMAX_DELAY
    );

    esp_err_t result = ESP_FAIL;

    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(WIFI_TAG, "Connected to AP: %s", WIFI_SSID);

        result = ESP_OK;
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGW(WIFI_TAG, "Failed to connect to: %s", WIFI_SSID);
    }
    else
    {
        ESP_LOGE(WIFI_TAG, "Unexpected event result");
    }

    ESP_LOGI(WIFI_TAG, "End wait for connection...");

    return result;
}

/**
 * @brief Initialize the ESP32 as a wi-fi station * 
 */
esp_err_t wifi_init_sta(void)
{
    ESP_LOGI(WIFI_TAG, "Begin wi-fi setup, connecting to: %s", WIFI_SSID);

    wifi_event_group = xEventGroupCreate();

    wifi_sta_init_phase();

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    wifi_sta_setup_events(&instance_any_id, &instance_got_ip);

    wifi_sta_config_phase();

    ESP_ERROR_CHECK(esp_wifi_start());

    esp_err_t result = wifi_sta_wait_connection();

    wifi_sta_cleanup_events(&instance_any_id, &instance_got_ip);
    vEventGroupDelete(wifi_event_group);

    ESP_LOGI(WIFI_TAG, "End wi-fi setup, result: %d", result);

    return result;
}