/**
 * @brief Functions for the oled screen. The u8g2 graphics library is used, which
 * manages the communication with the screen. Code is based on nkolban's configuration.
 *
 * https://github.com/olikraus/u8g2
 * https://github.com/nkolban/esp32-snippets/tree/master/hardware/displays/U8G2
 */

#include "oled.h"

/**
 * @brief Draw multiple lines on the oled screen. The lines are centered
 * both horizontally and vertically.
 * 
 * @param text Array with lines to show
 * @param lines The number of lines
 */
void oled_draw_centered_text(char** text, uint8_t lines)
{
    u8g2_ClearBuffer(&oled);
    u8g2_uint_t font_height = u8g2_GetMaxCharHeight(&oled);
    u8g2_uint_t display_width = u8g2_GetDisplayWidth(&oled);
    u8g2_uint_t display_height = u8g2_GetDisplayHeight(&oled);

    u8g2_uint_t yPos = (display_height - font_height * lines) / 2 + font_height;

    for (uint8_t line = 0; line < lines; line++)
    {
        u8g2_uint_t xPos = (display_width - u8g2_GetUTF8Width(&oled, text[line])) / 2;

        u8g2_DrawUTF8(&oled, xPos, yPos, text[line]);
        yPos += font_height;
    }

    u8g2_SendBuffer(&oled);
}

/**
 * @brief Draw a product to the screen
 * 
 * @param name Name of the product
 * @param price Price of the product (euros is assumed)
 */
void oled_draw_product(char* name, double price)
{
    // Convert the price into a string
    char price_str[6] = {0};
    sprintf(price_str, "%2.2f", price);

    // sprintf does not like '€' in the format string, so it is prepended seperately
    char total_price_str[10] = "€";
    strcat(total_price_str, price_str);

    char *text[2] = {
        name,
        total_price_str
    };

    oled_draw_centered_text(text, 2);
}

/**
 * @brief Initialize the oled screen, set the font, disable power saving and clear the screen * 
 */
void oled_init(void)
{
    ESP_LOGI(OLED_TAG, "Begin oled init");

    u8g2_Setup_sh1106_i2c_128x64_noname_f(
        &oled,
        U8G2_R0,
        u8g2_esp32_i2c_byte_cb,
        u8g2_esp32_gpio_and_delay_cb
    );

    u8g2_SetI2CAddress(&oled.u8x8, OLED_ADDR);
    u8g2_InitDisplay(&oled);
    u8g2_SetPowerSave(&oled, false);
    u8g2_ClearBuffer(&oled);
    u8g2_SetFont(&oled, u8g2_font_unifont_t_symbols);
    u8g2_SendBuffer(&oled);

    ESP_LOGI(OLED_TAG, "End oled init");
}

/**
 * @brief Callback for the u8g2 library to send I2C messages
 * 
 * @param u8x8 u8x8 instance
 * @param msg The action to perform
 * @param arg_int The number of parameters
 * @param arg_ptr Paremeters for the action
 * @return uint8_t 
 */
uint8_t u8g2_esp32_i2c_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    // ESP_LOGI("u8g2_hal", "i2c call msg: %d, arg_int: %d", msg, arg_int);

    switch (msg)
    {
        case U8X8_MSG_BYTE_INIT: {
            // Initialize I2C
            i2c_config_t conf = {
                .mode = I2C_MODE_MASTER,
                .sda_io_num = 21,
                .sda_pullup_en = GPIO_PULLUP_ENABLE,
                .scl_io_num = 22,
                .scl_pullup_en = GPIO_PULLUP_ENABLE,
                .master.clk_speed = I2C_MASTER_FREQ_HZ
            };

            ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
            ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));
            break;
        };
        case U8X8_MSG_BYTE_SET_DC: {
            // NOOP
            break;
        };
        case U8X8_MSG_BYTE_START_TRANSFER: {
            // Send start bit and device address
            i2c_handle = i2c_cmd_link_create();
            ESP_ERROR_CHECK(i2c_master_start(i2c_handle));
            ESP_ERROR_CHECK(i2c_master_write_byte(i2c_handle, u8x8_GetI2CAddress(u8x8) | I2C_MASTER_WRITE, 1));

            break;
        };
        case U8X8_MSG_BYTE_SEND: {
            // Send message bytes
            uint8_t *data = (uint8_t*) arg_ptr;

            while (arg_int > 0)
            {
                ESP_ERROR_CHECK(i2c_master_write_byte(i2c_handle, *data, 1));
                arg_int--;
                data++;
            }
            break;
        };
        case U8X8_MSG_BYTE_END_TRANSFER: {
            // Send stop bit
            ESP_ERROR_CHECK(i2c_master_stop(i2c_handle));
            // Execute the command
            ESP_ERROR_CHECK(
                i2c_master_cmd_begin(I2C_MASTER_NUM, i2c_handle, I2C_TIMEOUT_MS / portTICK_RATE_MS)
            );
            // Cleanup
            i2c_cmd_link_delete(i2c_handle);
            break;
        };
        default:
            return 0;
    }

    return 1;
}

/**
 * @brief This method is used by the u8g2 library to perform bit-banged I2C/SPI. But since
 * I2C hardware is used the method can be left empty.
 */
uint8_t u8g2_esp32_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    // NOOP
    // ESP_LOGI("u8g2_hal", "gpio/delay call msg: %d, arg_int: %d", msg, arg_int);
    return 1;
}