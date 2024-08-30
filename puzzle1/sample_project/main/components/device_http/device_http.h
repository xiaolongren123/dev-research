#ifndef __DEVICE_HTTP_H__
#define __DEVICE_HTTP_H__

#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_event.h"
#include "cJSON.h"

typedef struct device_http_handle_t
{
    char *url;
    char *json_item;
    char *response_body;
    char *json_item_data;
    void (*http_init)(struct device_http_handle_t *dev);
    esp_err_t (*http_get_json_item)(struct device_http_handle_t *dev);
    void *private_data;
} device_http_handle_t, *p_device_http_handle_t;

device_http_handle_t *GetHTTPDevice(void);

#endif