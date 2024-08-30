#include "device_usart.h"

static const char *TAG = "USART";

static const int RX_BUF_SIZE = 128;
static const int TX_BUF_SIZE = 1024;
static const int UART_TX_CHUNK_SIZE = 256;
static const int DEFAULT_USART_RATE = 115200;

/*
 * @brief 初始化 usart 设备
 * @param 设备句柄,用于传入串口配置参数
 * @return ESP_OK：执行成功，ESP_FAIL：执行失败 
 */
esp_err_t usart_init(struct device_usart_handle_t *usart_dev)
{

    esp_err_t ret = ESP_OK;

    const uart_config_t uart_config = {
        .baud_rate = usart_dev->usart_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ret = uart_driver_install(usart_dev->usart_num, RX_BUF_SIZE * 2, TX_BUF_SIZE * 2, 20, NULL, 0);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to install UART driver: %s", esp_err_to_name(ret));
        return ret;
    }
    ret = uart_param_config(usart_dev->usart_num, &uart_config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to configure UART: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = uart_set_pin(usart_dev->usart_num, usart_dev->txd_gpio_num, usart_dev->rxd_gpio_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set UART pins: %s", esp_err_to_name(ret));
        return ret;
    }

    usart_dev->rx_queue = xQueueCreate(20, sizeof(char));//创建接收消息队列
        if (usart_dev->rx_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create RX queue");
        return ESP_FAIL;
    }
    return ESP_OK;
}

/*
 * @brief 串口发送函数
 * @param 设备句柄,发送长度
 * @return -1为错误，其他为发送长度
 */
static int usart_send(struct device_usart_handle_t *usart_dev, char *data, int length)
{
    int ret=0;
    int sent = 0;
    while (sent < length)
    {
        int remaining = length - sent;
        int chunk_size;

        if (remaining > UART_TX_CHUNK_SIZE)//分块发送，避免读取文件过大
        {
            chunk_size = UART_TX_CHUNK_SIZE;
        }
        else
        {
            chunk_size = remaining;
        }
        ret=uart_write_bytes(usart_dev->usart_num, data + sent, chunk_size);
        if(ret==-1){
            return ret;
        }
        sent += chunk_size;
    }
    uart_write_bytes(usart_dev->usart_num, "\n", 1);
    return ret;
}

/*
 * @brief 将接收到的数据保存在消息队列中
 * @param 设备句柄,接收缓冲区,接收长度
 * @return 
 */
static void usart_recv(struct device_usart_handle_t *usart_dev)
{
    uint8_t data[RX_BUF_SIZE];
    int length = 0;
    int total_length = 0;
    char buffer[256] = {0};

    while (1) {
        length = uart_read_bytes(usart_dev->usart_num, data, RX_BUF_SIZE, portMAX_DELAY);
        if (length > 0) {
            for (int i = 0; i < length; i++) {
                buffer[total_length++] = data[i];
                if (data[i] == '\n') {
                    buffer[total_length] = '\0'; 
                    xQueueSend(usart_dev->rx_queue, buffer, portMAX_DELAY);
                    total_length = 0;
                }
            }
        }
    }
}


//初始化串口设备
static device_usart_handle_t g_usart0_dev = {
    .usart_num = UART_NUM_0,
    .usart_rate = DEFAULT_USART_RATE,
    .txd_gpio_num = UART_PIN_NO_CHANGE,
    .rxd_gpio_num = UART_PIN_NO_CHANGE,
    .rx_queue = NULL,
    .usart_init = usart_init,
    .usart_send = usart_send,
    .usart_recv = usart_recv,
};

static device_usart_handle_t g_usart1_dev = {
    .usart_num = UART_NUM_1,
    .usart_rate = DEFAULT_USART_RATE,
    .txd_gpio_num = UART_PIN_NO_CHANGE,
    .rxd_gpio_num = UART_PIN_NO_CHANGE,
    .rx_queue = NULL,
    .usart_init = usart_init,
    .usart_send = usart_send,
    .usart_recv = usart_recv,
};

static device_usart_handle_t g_usart2_dev = {
    .usart_num = UART_NUM_2,
    .usart_rate = DEFAULT_USART_RATE,
    .txd_gpio_num = UART_PIN_NO_CHANGE,
    .rxd_gpio_num = UART_PIN_NO_CHANGE,
    .rx_queue = NULL,
    .usart_init = usart_init,
    .usart_send = usart_send,
    .usart_recv = usart_recv,};

//串口设备数组,通过串口号识别串口设备
static struct device_usart_handle_t *g_uart_devices[] = {&g_usart0_dev, &g_usart1_dev, &g_usart2_dev};

/*
 * @brief 获取串口设备句柄
 * @param 无
 * @return 串口设备句柄
 */
device_usart_handle_t *GetUSARTDevice(int usart_num)
{
    int i = 0;
    for (i = 0; i < sizeof(g_uart_devices) / sizeof(g_uart_devices[0]); i++)
    {
            return g_uart_devices[usart_num];
    }
    return NULL;
}