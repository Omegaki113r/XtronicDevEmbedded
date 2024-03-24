#include <zephyr/kernel.h>

#include <zephyr/drivers/gpio.h>

#define LED1_NODE DT_NODELABEL(led1)
#define LED2_NODE DT_NODELABEL(led2)

const struct gpio_dt_spec led1_spec = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
const struct gpio_dt_spec led2_spec = GPIO_DT_SPEC_GET(LED2_NODE, gpios);

struct k_thread led1_thread_data;
struct k_thread led2_thread_data;

K_THREAD_STACK_DEFINE(led1_stack, 500); // create a stack memory area of 500 bytes with the name of led1_stack
K_THREAD_STACK_DEFINE(led2_stack, 500);

void led1_thread_entry(void *, void *, void *);
void led2_thread_entry(void *, void *, void *);

int main(void)
{
        gpio_pin_configure_dt(&led1_spec, GPIO_OUTPUT_INACTIVE); // GPIO_OUTPUT_INACTIVE => starting state is LOW
        gpio_pin_configure_dt(&led2_spec, GPIO_OUTPUT_INACTIVE);

        k_tid_t led1_blink_thread_handle_id = k_thread_create(&led1_thread_data, led1_stack, K_THREAD_STACK_SIZEOF(led1_stack), led1_thread_entry, NULL, NULL, NULL, K_LOWEST_THREAD_PRIO, 0, K_NO_WAIT);
        k_tid_t led2_blink_thread_handle_id = k_thread_create(&led2_thread_data, led2_stack, K_THREAD_STACK_SIZEOF(led2_stack), led2_thread_entry, NULL, NULL, NULL, K_LOWEST_THREAD_PRIO, 0, K_NO_WAIT);
        return 0;
}

void led1_thread_entry(void *param1, void *param2, void *param3)
{
        for (;;)
        {
                gpio_pin_toggle_dt(&led1_spec);
                k_msleep(1000);
        }
}

void led2_thread_entry(void *param1, void *param2, void *param3)
{
        for (;;)
        {
                gpio_pin_toggle_dt(&led2_spec);
                k_msleep(1000);
        }
}