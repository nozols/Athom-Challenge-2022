#pragma once

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "esp_http_client.h"
#include "cJSON.h"

#define MAX_HTTP_OUTPUT_BUFFER 1024
#define API_HOST CONFIG_API_HOST
#define API_PORT CONFIG_API_PORT

static const char *HTTP_TAG = "HTTP_CLIENT";

esp_err_t http_event_handler(esp_http_client_event_t *evt);
esp_err_t http_get_request(char *path, char *response_buffer);
esp_err_t http_post_request(char *path, char *response_buffer, cJSON *data);