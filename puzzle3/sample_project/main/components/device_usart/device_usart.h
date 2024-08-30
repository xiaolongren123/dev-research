#ifndef Device_USART_H
#define Device_USART_H

#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"


typedef struct device_usart_handle_t
{
    int usart_num;
    int usart_rate;
    uint8_t txd_gpio_num;
    uint8_t rxd_gpio_num;
    QueueHandle_t rx_queue;
    esp_err_t (*usart_init)(struct device_usart_handle_t *usart_dev);
    int (*usart_send)(struct device_usart_handle_t *usart_dev, char *data, int length);
    void (*usart_recv)(struct device_usart_handle_t *usart_dev);
    void *private_data;
} device_usart_handle_t, *p_device_usart_handle_t;

device_usart_handle_t *GetUSARTDevice(int usart_num);

#endif