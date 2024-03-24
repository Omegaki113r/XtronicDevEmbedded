#include <stdio.h>
#include <string.h>

#include <driver/gpio.h>
#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#define BUTTON GPIO_NUM_23
#define LED GPIO_NUM_22

struct gpio_isr_process
{
    gpio_num_t gpio;
};
QueueHandle_t gpio_isr_queue_handle;

void isr_callback(void *);
void isr_process_task(void *);

void app_main(void)
{
    gpio_isr_queue_handle = xQueueCreate(5, sizeof(struct gpio_isr_process));
    if (gpio_isr_queue_handle == NULL)
    {
        ESP_LOGE(__func__, "Queue Creation failed..");
        return;
    }

    gpio_install_isr_service(0); // needed to detect interrupts using button presses. otherwise won't work

    gpio_reset_pin(BUTTON);
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
    gpio_set_intr_type(BUTTON, GPIO_INTR_NEGEDGE);
    gpio_intr_enable(BUTTON);
    gpio_isr_handler_add(BUTTON, isr_callback, (void *)BUTTON);

    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_level(LED, 0);

    xTaskCreate(isr_process_task, "isr_process_task", 1024 * 2, NULL, configMAX_PRIORITIES - 5, NULL); // need to create and run the task. otherwise the LED won't trigger
}

void isr_callback(void *arg)
{
    struct gpio_isr_process triggered_data = {};
    memcpy(&triggered_data.gpio, &arg, sizeof(gpio_num_t)); // this might not be the best way. but this is what i usually use

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(gpio_isr_queue_handle, &triggered_data, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR();
    }
}

void isr_process_task(void *args)
{
    struct gpio_isr_process triggered_data = {};
    for (;;)
    {
        xQueueReceive(gpio_isr_queue_handle, &triggered_data, portMAX_DELAY); // this will wait infinitely till a ISR sends a packet to Queue
        ESP_LOGI(__func__, "triggered pin was: %d", triggered_data.gpio);
        gpio_set_level(LED, !gpio_get_level(LED));
    }
    vTaskDelete(NULL);
}