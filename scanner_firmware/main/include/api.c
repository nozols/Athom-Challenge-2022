#include "api.h"

/**
 * @brief Turn the mac address of the device into a string.
 * It is used as unique id for the database.
 * 
 * @return esp_err_t 
 */
esp_err_t api_get_address(void)
{
    uint8_t raw[6] = {0};

    esp_err_t status = esp_base_mac_addr_get(raw);

    if (status != ESP_OK)
        return status;

    sprintf(mac_address_str, "%X:%X:%X:%X:%X:%X", raw[0], raw[1], raw[2], raw[3], raw[4], raw[5]);

    ESP_LOGI(API_TAG, "Mac address: %s", mac_address_str);

    return ESP_OK;
}


/**
 * @brief Request the name for an item from the API
 * 
 * @param nfc_id_str The id of the nfc tag that was scanned
 * @param item_name Place to put the item name
 * @param item_price Place to put the item price
 * @return esp_err_t ESP_FAIL if json response could not be parsed, also HTTP error code if applicable
 */
esp_err_t api_request_name(char *nfc_id_str, char *item_name, double *item_price)
{
    char buffer[API_REQUEST_BUFFER_SIZE] = {0};

    // Create the JSON object that is going to be send
    cJSON *root = cJSON_CreateObject();
    cJSON *nfc_id = cJSON_CreateString(nfc_id_str);
    cJSON *address = cJSON_CreateString(mac_address_str);
    cJSON_AddItemToObject(root, "nfcId", nfc_id);
    cJSON_AddItemToObject(root, "address", address);

    ESP_LOGI(API_TAG, "Sending json: \n%s", cJSON_Print(root));

    // Send the request
    esp_err_t response_status = http_post_request("/scan", buffer, root);

    if (response_status != ESP_OK)
        return response_status;

    // Parse the response into item_name and item_price
    cJSON *response_root = cJSON_Parse(buffer);

    if (response_root == NULL)
        return ESP_FAIL;

    const cJSON *name = cJSON_GetObjectItem(response_root, "name");

    if (cJSON_IsString(name) && name->valuestring != NULL)
    {
        ESP_LOGI(API_TAG, "Item name: %s", name->valuestring);
        strcpy(item_name, name->valuestring);
    }
    else
    {
        return ESP_FAIL;
    }

    const cJSON *price = cJSON_GetObjectItem(response_root, "price");

    if (cJSON_IsNumber(price))
    {
        ESP_LOGI(API_TAG, "Item price: %f", price->valuedouble);
        *item_price = price->valuedouble;
    }
    else
    {
        return ESP_FAIL;
    }

    cJSON_Delete(response_root);
    cJSON_Delete(root);

    return response_status;
}