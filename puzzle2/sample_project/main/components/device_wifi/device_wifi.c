#include "device_wifi.h"

static const char *TAG = "WIFI";

#define WIFI_SSID_DEFAULT "UPGRADE_AP"
#define WIFI_PASSWORD_DEFAULT "TEST1234"
#define WIFI_MAXIMUM_RETRY 100

/*
 * @brief WIFI 事件处理函数，内部调用
 * @param 
 * @return 无
 */
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    static int s_retry_num = 0;
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < WIFI_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
    }
}

/*
 * @brief 初始化 WIFI
 * @param 设备句柄,用于传入WiFi名和密码
 * @return ESP_OK：执行成功，ESP_FAIL：执行失败 
 */
static esp_err_t wifi_init(struct device_wifi_handle_t *dev)
{
    esp_err_t ret = ESP_OK;
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &event_handler,
                                        NULL,
                                        &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT,
                                        IP_EVENT_STA_GOT_IP,
                                        &event_handler,
                                        NULL,
                                        &instance_got_ip);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "",
            .password = ""},
    };
    strncpy((char *)wifi_config.sta.ssid, dev->ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char *)wifi_config.sta.password, dev->password, sizeof(wifi_config.sta.password) - 1);

    esp_wifi_set_mode(WIFI_MODE_STA);
    ret = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);

    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    ESP_LOGI(TAG, "wifi_init finished.");
    return ret;
}

/*
 * @brief 断开 WIFI，系统API封装
 * @param 无
 * @return ESP_OK：执行成功，ESP_FAIL：执行失败 
 */
static esp_err_t custom_wifi_deinit(void)
{
    return esp_wifi_deinit();
}

//全局 WIFI 设备句柄，用于外部调用
static struct device_wifi_handle_t g_wifi_dev =
    {
        .ssid = WIFI_SSID_DEFAULT,
        .password = WIFI_PASSWORD_DEFAULT,
        wifi_init,
        custom_wifi_deinit,
};

/*
 * @brief 获取WIFI 设备句柄
 * @param 无
 * @return WIFI 设备句柄
 */
device_wifi_handle_t *GetWiFiDevice(void)
{
    return &g_wifi_dev;
}