#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#define LED1_NODE DT_NODELABEL(led1)
#define LED2_NODE DT_NODELABEL(led2)

#define BUTTON1_NODE DT_NODELABEL(button1)

const struct gpio_dt_spec led1_spec = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
const struct gpio_dt_spec led2_spec = GPIO_DT_SPEC_GET(LED2_NODE, gpios);

struct gpio_callback gpio_pin_callback_data;
void gpio_isr_callback(const struct device *device, struct gpio_callback *cb, uint32_t pin);
const struct gpio_dt_spec button1_spec = GPIO_DT_SPEC_GET(BUTTON1_NODE, gpios);

struct k_thread led1_thread_data;
K_THREAD_STACK_DEFINE(led1_stack, 500);
void led1_thread(void *, void *, void *);

struct k_thread isr_process_thread_data;
K_THREAD_STACK_DEFINE(isr_process_stack, 500);
void isr_process_thread(void *, void *, void *);

struct triggered_gpio
{
        uint32_t gpio;
};
K_MSGQ_DEFINE(isr_process_queue, sizeof(struct triggered_gpio), 5, 1);

int main(void)
{
        // configuring LEDs
        gpio_pin_configure_dt(&led1_spec, GPIO_OUTPUT_INACTIVE);
        gpio_pin_configure_dt(&led2_spec, GPIO_OUTPUT_INACTIVE);

        // configuring button
        gpio_pin_configure_dt(&button1_spec, GPIO_INPUT);
        gpio_pin_interrupt_configure_dt(&button1_spec, GPIO_INT_EDGE_FALLING);
        gpio_init_callback(&gpio_pin_callback_data, gpio_isr_callback, BIT(button1_spec.pin));
        gpio_add_callback_dt(&button1_spec, &gpio_pin_callback_data);

        // Toggle 1 LED always with some interval
        k_tid_t led1_thread_handle_id = k_thread_create(&led1_thread_data, led1_stack, K_THREAD_STACK_SIZEOF(led1_stack), led1_thread, NULL, NULL, NULL, K_LOWEST_THREAD_PRIO, 0, K_NO_WAIT);

        // we will process the isr in here
        k_tid_t isr_process_thread_handle_id = k_thread_create(&isr_process_thread_data, isr_process_stack, K_THREAD_STACK_SIZEOF(isr_process_stack), isr_process_thread, NULL, NULL, NULL, K_LOWEST_THREAD_PRIO + 1, 0, K_NO_WAIT);
}

void gpio_isr_callback(const struct device *device, struct gpio_callback *cb, uint32_t pin)
{
        struct triggered_gpio triggered = {
            .gpio = pin,
        };
        k_msgq_put(&isr_process_queue, &triggered, K_NO_WAIT);
}

void led1_thread(void *arg1, void *arg2, void *arg3)
{
        for (;;)
        {
                gpio_pin_toggle_dt(&led1_spec);
                k_msleep(500);
        }
}

void isr_process_thread(void *arg1, void *arg2, void *arg3)
{
        for (;;)
        {
                struct triggered_gpio triggered = {};
                k_msgq_get(&isr_process_queue, &triggered, K_FOREVER);
                if (triggered.gpio == button1_spec.pin)
                {
                        gpio_pin_toggle_dt(&led2_spec);
                }
        }
}