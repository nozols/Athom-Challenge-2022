#include "http.h"

/**
 * @brief Handle HTTP events
 * This method is copied from the HTTP example:
 * https://github.com/espressif/esp-idf/blob/master/examples/protocols/esp_http_client/main/esp_http_client_example.c
 * @param evt 
 * @return esp_err_t 
 */
esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;
    static int output_len;

    switch (evt->event_id)
    {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);

            if (!esp_http_client_is_chunked_response(evt->client))
            {
                if (evt->user_data)
                {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                }
                else
                {
                    if (output_buffer == NULL)
                    {
                        output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                        output_len = 0;

                        if (output_buffer == NULL)
                        {
                            ESP_LOGE(HTTP_TAG, "Could not allocate memory for output buffer!");
                            return ESP_FAIL;
                        }
                    }

                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }

                output_len += evt->data_len;
            }

            break;

        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(HTTP_TAG, "HTTP_EVENT_ON_FINISH");

            if (output_buffer != NULL) 
            {
                free(output_buffer);
                output_buffer = NULL;
            }

            output_len = 0;

            break;
        
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_DISCONNECTED");

            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);

            if (err != 0)
            {
                ESP_LOGI(HTTP_TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(HTTP_TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }

            if (HTTP_TAG != NULL)
            {
                free(output_buffer);
                output_buffer = NULL;
            }

            output_len = 0;

            break;
    }

    return ESP_OK;
}

/**
 * @brief Perform a POST request
 * 
 * @param path The path to send the request to
 * @param response_buffer Resulting response data
 * @param data The data to send in JSON format
 * @return esp_err_t response status
 */
esp_err_t http_post_request(char *path, char *response_buffer, cJSON *data)
{
    esp_http_client_config_t config = {
        .host = API_HOST,
        .port = API_PORT,
        .path = path,
        .event_handler = http_event_handler,
        .user_data = response_buffer,
        .disable_auto_redirect = true,
        .method = HTTP_METHOD_POST,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    char *json_string = cJSON_Print(data);

    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, json_string, strlen(json_string));

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK)
    {
        ESP_LOGI(
            HTTP_TAG,
            "HTTP GET status = %d, content_length = %11d",
            esp_http_client_get_status_code(client),
            esp_http_client_get_content_length(client)
        );
    }
    else
    {
        ESP_LOGE(
            HTTP_TAG,
            "HTTP GET request failed: %s (%s:%d%s) status: %d",
            esp_err_to_name(err),
            API_HOST,
            API_PORT,
            path,
            esp_http_client_get_status_code(client)    
        );
    }

    esp_http_client_cleanup(client);

    return err;
}