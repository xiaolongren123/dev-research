#ifndef DEVICE_WIFI_H
#define DEVICE_WIFI_H

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "string.h"

typedef struct device_wifi_handle_t
{
    char *ssid;
    char *password;
    esp_err_t (*wifi_init)(struct device_wifi_handle_t *dev);
    esp_err_t (*wifi_deinit)(void);
} device_wifi_handle_t, *p_device_wifi_handle_t;

device_wifi_handle_t *GetWiFiDevice(void);

#endif
