#include <stdio.h>
#include "nvs_flash.h"

#include "device_wifi.h"
#include "device_http.h"

#define WIFI_SSID "UPGRADE_AP"
#define WIFI_PASSWORD "TEST1234"

#define URL "https://dummyjson.com/products/1"
#define JSON_ITEM "brand"

static const char *TAG = "puzzle1";

device_wifi_handle_t *wifi_dev;
device_http_handle_t *http_dev;
esp_err_t ret;

/*
 * @brief 初始化分区和所有设备
 * @param 无
 * @return 无
 */
void app_main_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_dev = GetWiFiDevice();

    wifi_dev->ssid = WIFI_SSID;
    wifi_dev->password = WIFI_PASSWORD;

    http_dev = GetHTTPDevice();

    http_dev->url = URL;
    http_dev->json_item = JSON_ITEM;
}
void app_main(void)
{
    app_main_init();

    ret = wifi_dev->wifi_init(wifi_dev);//初始化并连接WiFi

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize Wi-Fi: %s", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "Wi-Fi initialized successfully.");

        http_dev->http_init(http_dev);

        ret = http_dev->http_get_json_item(http_dev);//获取json数据

        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to get json item: %s", esp_err_to_name(ret));
        }
        else
        {
            ESP_LOGI(TAG, "Get json item successfully.");
            ESP_LOGI(TAG, "Json item: %s", http_dev->json_item_data);//输出json数据

            wifi_dev->wifi_deinit();//断开wifi
        }
    }
    while (1)
    {
        ;
    }
}
