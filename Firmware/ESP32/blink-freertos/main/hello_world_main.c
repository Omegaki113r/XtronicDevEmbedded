#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <driver/gpio.h>

#define LED1 GPIO_NUM_18
#define LED2 GPIO_NUM_19

void configure_gpio_as_output(gpio_num_t, uint32_t);

void blink_led1_task(void *);
void blink_led2_task(void *);

void app_main(void)
{
    configure_gpio_as_output(LED1, 0);
    configure_gpio_as_output(LED2, 0);

    xTaskCreate(blink_led1_task, "blink_led1_task", 1024 * 1, NULL, configMAX_PRIORITIES - 5, NULL);
    xTaskCreate(blink_led2_task, "blink_led2_task", 1024 * 1, NULL, configMAX_PRIORITIES - 5, NULL);
}

void configure_gpio_as_output(gpio_num_t gpio_num, uint32_t default_level)
{
    gpio_reset_pin(gpio_num);
    gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
    gpio_set_level(gpio_num, default_level);
}

void blink_led1_task(void *params)
{
    for (;;)
    {
        gpio_set_level(LED1, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(LED1, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    vTaskDelete(NULL);
}

void blink_led2_task(void *params)
{
    for (;;)
    {
        gpio_set_level(LED2, 1);
        vTaskDelay(pdMS_TO_TICKS(250));
        gpio_set_level(LED2, 0);
        vTaskDelay(pdMS_TO_TICKS(250));
    }
    vTaskDelete(NULL);
}