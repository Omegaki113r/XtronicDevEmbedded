#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

#include <driver/gpio.h>

#define LED1 GPIO_NUM_15
#define LED2 GPIO_NUM_16

TimerHandle_t timer1_handle, timer2_handle;

void timer1_cb(TimerHandle_t);
void timer2_cb(TimerHandle_t);

void app_main(void)
{
    gpio_reset_pin(LED1);
    gpio_set_direction(LED1,
                       GPIO_MODE_INPUT_OUTPUT // this was made to input+output so we can toggle the GPIO without
                                              // managing a seperate variable for storing the current state
    );
    gpio_set_level(LED1, 0);
    gpio_reset_pin(LED2);
    gpio_set_direction(LED2, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_level(LED2, 0);

    timer1_handle = xTimerCreate(
        "timer1",           // name for the timer. ESPIDF will use this to log some information
        pdMS_TO_TICKS(500), // how long the period is. assuming that this is a repeating timer period is the
                            // interval between each callback trigger
        pdTRUE,             // whether the internal counter needs to be reset to 0 after the callback has triggered
        NULL,               // Some kind of data that needs to be passed into the callback
        timer1_cb           // actual user callback
    );
    timer2_handle = xTimerCreate("timer2", pdMS_TO_TICKS(250), pdTRUE, NULL, timer2_cb);
    xTimerStart(timer1_handle,
                portMAX_DELAY // this will block the entier application till the timer starts.
                              // for this application blocking the function call is OK. Since there is no other tasks/timers running
    );
    xTimerStart(timer2_handle, portMAX_DELAY);
    for (;;)
    {
        vTaskDelay((pdMS_TO_TICKS(1000)));
    }
}

void timer1_cb(TimerHandle_t timer_handle)
{
    gpio_set_level(LED1, !gpio_get_level(LED1));
}
void timer2_cb(TimerHandle_t timer_handle)
{
    gpio_set_level(LED2, !gpio_get_level(LED2));
}