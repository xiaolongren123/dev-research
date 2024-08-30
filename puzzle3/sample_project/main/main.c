#include <stdio.h>
#include "nvs_flash.h"

#include "device_wifi.h"
#include "device_http.h"
#include "device_flash.h"
#include "device_usart.h"

#define WIFI_SSID "UPGRADE_AP"
#define WIFI_PASSWORD "TEST1234"

#define URL "https://dummyjson.com/products/1"
#define JSON_ITEM "brand"

#define FILE_NAME "hello_this_is_long_name_products.json"

#define USART_NUM UART_NUM_1
#define USART_RATE 115200
#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_5)

static const char *TAG = "puzzle3";

device_wifi_handle_t *wifi_dev;
device_http_handle_t *http_dev;
device_flash_handle_t *flash_dev;
device_usart_handle_t *usart_dev;
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

    flash_dev = GetFLASHDevice();
    flash_dev->flash_init();

    usart_dev = GetUSARTDevice(USART_NUM);
    usart_dev->usart_num = USART_NUM;
    usart_dev->usart_rate = USART_RATE;
    usart_dev->txd_gpio_num = TXD_PIN;
    usart_dev->rxd_gpio_num = RXD_PIN;
    usart_dev->usart_init(usart_dev);
}

/*
 * @brief 串口触发的执行函数，包括存储json数据并通过串口1发送
 * @param 无
 * @return 无
 */
void http_fun(void)
{
    ret = wifi_dev->wifi_init(wifi_dev);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize Wi-Fi: %s", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "Wi-Fi initialized successfully.");
        http_dev->http_init(http_dev);

        ret = http_dev->http_get_json_item(http_dev);

        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to get json item: %s", esp_err_to_name(ret));
        }
        else
        {
            ESP_LOGI(TAG, "Get json item successfully.");
            ESP_LOGI(TAG, "Json item: %s", http_dev->json_item_data);
            ret = flash_dev->flash_write(FILE_NAME, http_dev->response_body);
            if (ret != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to save file: %s", esp_err_to_name(ret));
            }
            else
            {
                char *buffer = NULL;
                int length = 0;
                ESP_LOGI(TAG, "File saved successfully.");
                if (flash_dev->flash_read(FILE_NAME, &buffer, length) != ESP_OK)//读取文件
                {
                    ESP_LOGE(TAG, "Failed to read from flash");
                }
                else
                {
                    usart_dev->usart_send(usart_dev, buffer, length);
                }
            }
        }
        wifi_dev->wifi_deinit();
    }
}

void uart_receive_task(void *pvParameters)
{
    char data[128 * 2];//接收缓冲区

    while (1)
    {
        if (xQueueReceive(usart_dev->rx_queue, data, portMAX_DELAY))
        {
            if (strstr(data, "fetch") != NULL)//对比指令是否为fetch
            {
                ESP_LOGI(TAG, "Received 'fetch' command");
                http_fun();
            }
            else
            {
                ESP_LOGI(TAG, "Received data: %s", data);
            }
        }
    }
}

void app_main(void)
{
    app_main_init();
    usart_dev->usart_recv(usart_dev);//调用接收方法
    xTaskCreate(uart_receive_task, "uart_receive_task", 4096, NULL, 10, NULL);
}
