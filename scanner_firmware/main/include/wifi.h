#pragma once
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_log.h"

#define WIFI_SSID CONFIG_ESP_WIFI_SSID
#define WIFI_PASS CONFIG_ESP_WIFI_PASSWORD
#define WIFI_RETR CONFIG_ESP_MAXIMUM_RETRY

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *WIFI_TAG = "network/wifi";

/** Keep track of the total retries */
static int wifi_retries = 0;

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t wifi_event_group;

void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void wifi_sta_init_phase(void);
void wifi_sta_setup_events(esp_event_handler_instance_t *instance_any_id, esp_event_handler_instance_t *instance_got_ip);
void wifi_sta_cleanup_events(esp_event_handler_instance_t *instance_any_id, esp_event_handler_instance_t *instance_got_ip);
void wifi_sta_config_phase(void);
esp_err_t wifi_sta_wait_connection(void);
esp_err_t wifi_init_sta(void);
