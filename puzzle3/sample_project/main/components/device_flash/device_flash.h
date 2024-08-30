#ifndef _DEVICE_FLASH_H_
#define _DEVICE_FLASH_H_

#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_spiffs.h"
#include <stdio.h>
#include <string.h>


typedef struct device_flash_handle_t {
    esp_err_t (*flash_init)(void);
    esp_err_t (*flash_write)(const char *file_name, const char *data);
    esp_err_t (*flash_read)(const char *file_name, char **buffer, unsigned int *length);
} device_flash_handle_t, *p_device_flash_handle_t;

device_flash_handle_t *GetFLASHDevice(void);

#endif