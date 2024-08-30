#include "device_flash.h"

static const char *TAG = "Flash";

/*
 * @brief flash初始化函数
 * @param 
 * @return ESP_OK：执行成功，ESP_FAIL：执行失败 
 */
static esp_err_t flash_init(void)
{
    if (!esp_spiffs_mounted(NULL))
    {
        ESP_LOGI(TAG, "Initializing SPIFFS");

        const esp_vfs_spiffs_conf_t conf = {
            .base_path = "/spiffs",
            .partition_label = NULL,
            .max_files = 5,
            .format_if_mount_failed = false};

        esp_err_t ret = esp_vfs_spiffs_register(&conf);
        if (ret != ESP_OK)
        {
            if (ret == ESP_FAIL)
            {
                ESP_LOGE(TAG, "Failed to mount or format filesystem");
            }
            else if (ret == ESP_ERR_NOT_FOUND)
            {
                ESP_LOGE(TAG, "Failed to find SPIFFS partition");
            }
            else
            {
                ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
            }
            return ESP_FAIL;
        }

        size_t total = 0, used = 0;
        esp_spiffs_info(NULL, &total, &used);
        ESP_LOGI(TAG, "Total: %dKB, Used: %dKB", total / 1024, used / 1024);
    }
    return ESP_OK;
}

/*
 * @brief Flash写数据
 * @param 文件名和数据地址
 * @return ESP_OK：执行成功，ESP_FAIL：执行失败 
 */
static esp_err_t flash_write(const char *file_name, const char *data)
{
    char path[64];
    snprintf(path, sizeof(path), "/spiffs/%s", file_name);

    FILE *fp = fopen(path, "w");
    if (fp != NULL)
    {
        fwrite(data, 1, strlen(data), fp);
        fclose(fp);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }
    return ESP_OK;
}

/*
 * @brief Flash读数据
 * @param 文件名，buffer指针，长度
 * @return ESP_OK：执行成功，ESP_FAIL：执行失败 
 */
static esp_err_t flash_read(const char *file_name, char **buffer, unsigned int *length)
{
    char path[64];
    snprintf(path, sizeof(path), "/spiffs/%s", file_name);

    FILE *fp = fopen(path, "r");
    if (fp != NULL)
    {
        fseek(fp, 0, SEEK_END);
        *length = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        *buffer = (char *)malloc(*length + 1);
        if (*buffer == NULL)
        {
            ESP_LOGE(TAG, "Failed to allocate memory for read buffer");
            fclose(fp);
            return ESP_FAIL;
        }

        fread(*buffer, 1, *length, fp);
        (*buffer)[*length] = '\0';
        fclose(fp);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }
    return ESP_OK;
}

//初始化一个Flash设备句柄
static struct device_flash_handle_t g_flash_dev = {
    .flash_init = flash_init,
    .flash_write = flash_write,
    .flash_read = flash_read,
};

/*
 * @brief 获取对外设备句柄
 * @param 
 * @return 设备句柄
 */
device_flash_handle_t *GetFLASHDevice(void)
{
    return &g_flash_dev;
}

