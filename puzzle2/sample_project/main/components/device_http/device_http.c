#include "device_http.h"

static const char *TAG = "HTTP_CLIENT";

#define URL_DEFAULT "https://dummyjson.com/products/1"
#define JSON_ITEM_DEFAULT "brand"


//设备私有数据，内部调用
typedef struct priv_data
{
    esp_http_client_handle_t client;
} priv_data, *priv_data_t;

priv_data priv = {
    NULL,
};

/*
 * @brief HTTP连接事件处理函数
 * @param 
 * @return ESP_OK：执行成功，ESP_FAIL：执行失败 
 */
static esp_err_t event_handler(esp_http_client_event_t *evt)
{
    static char *response_body = NULL;
    static int body_len = 0;

    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        if (!response_body)
        {
            if (!response_body)
            {
                ESP_LOGE(TAG, "Failed to allocate memory for response body");
                return ESP_FAIL;
            }
        }
        else
        {
            if (!response_body)
            {
                ESP_LOGE(TAG, "Failed to reallocate memory for response body");
                return ESP_FAIL;
            }
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH, len=%d", body_len);
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        break;
    }
    return ESP_OK;
}

/*
 * @brief 初始化 HTTP设备
 * @param 设备句柄,用于传入私有数据和url
 * @return 无
 */
static void http_init(struct device_http_handle_t *dev)
{
    priv_data_t priv_data = dev->private_data;
    esp_http_client_config_t config = {
        .url = dev->url,
        .event_handler = event_handler,
    };
    priv_data->client = esp_http_client_init(&config);
}

/*
 * @brief 从设备获取并保存JSON全部数据，解析JSON数据
 * @param 设备句柄,用于传入私有数据和获取JSON数据
 * @return ESP_OK：执行成功，ESP_FAIL：执行失败 
 */
static esp_err_t http_get_json_item(struct device_http_handle_t *dev)
{
    priv_data_t priv_data = dev->private_data;

    esp_err_t err = esp_http_client_perform(priv_data->client);

    if (err == ESP_OK)
    {
        int content_length = esp_http_client_get_content_length(priv_data->client);
        char *buffer = (char *)malloc(content_length + 1);//分配空间
        if (buffer == NULL)
        {
            ESP_LOGE(TAG, "Failed to allocate memory for response buffer");
            esp_http_client_cleanup(priv_data->client);//清楚连接
            return ESP_FAIL;
        }
        esp_http_client_read(priv_data->client, buffer, content_length);
        buffer[content_length] = '\0';

        dev->response_body = buffer;//保存全部json数据

        cJSON *json = cJSON_Parse(buffer);
        if (json == NULL)
        {
            ESP_LOGE(TAG, "Failed to parse JSON");
            return ESP_FAIL;
        }
        else
        {
            cJSON *json_item_data = cJSON_GetObjectItem(json, dev->json_item);//获取指定的json数据
            if (cJSON_IsString(json_item_data) && (json_item_data->valuestring != NULL))
            {
                dev->json_item_data =json_item_data->valuestring;
            }
            cJSON_Delete(json);
        }
        free(buffer);//释放空间
    }
    return ESP_OK;
}

//初始化一个HTTP设备句柄
static struct device_http_handle_t g_http_dev = {
    .url = URL_DEFAULT,
    .json_item = JSON_ITEM_DEFAULT,
    .response_body = NULL,
    .json_item_data = NULL,
    http_init,
    http_get_json_item,
    .private_data = &priv,
};

/*
 * @brief 获取对外设备句柄
 * @param 
 * @return 设备句柄
 */
device_http_handle_t *GetHTTPDevice()
{
    return &g_http_dev;
}
