#pragma once
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "cJSON.h"
#include "http.h"

#define API_REQUEST_BUFFER_SIZE 1024

static const char *API_TAG = "API";
static char mac_address_str[18] = {0};

esp_err_t api_get_address();
esp_err_t api_request_name(char *nfc_id_str, char *item_name, double *item_price);