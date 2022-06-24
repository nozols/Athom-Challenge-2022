#pragma once
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "u8g2.h"

#define OLED_ADDR 0x78
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_TIMEOUT_MS 1000
#define I2C_MASTER_FREQ_HZ 400000

static const char* OLED_TAG = "OLED";

i2c_cmd_handle_t i2c_handle;
u8g2_t oled;

void oled_init(void);
void oled_draw_centered_text(char** text, uint8_t lines);
void oled_draw_product(char *name, double price);
uint8_t u8g2_esp32_i2c_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8g2_esp32_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);