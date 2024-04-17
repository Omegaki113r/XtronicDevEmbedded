#include <zephyr/kernel.h>

#include <zephyr/drivers/gpio.h>

// defining and acquiring device tree configs related to the GPIO OUTPUTs
#define LED0_NODE DT_NODELABEL(led0)
#define LED1_NODE DT_NODELABEL(led1)
#define LED2_NODE DT_NODELABEL(led2)
const struct gpio_dt_spec led0_spec = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
const struct gpio_dt_spec led1_spec = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
const struct gpio_dt_spec led2_spec = GPIO_DT_SPEC_GET(LED2_NODE, gpios);

// declaring software timer handles
struct k_timer led0_timer;
struct k_timer led1_timer;
struct k_timer led2_timer;
// declaring software timer callbacks that will be called after each expiration of the timer
void led0_timer_callback(struct k_timer *timer_id);
void led1_timer_callback(struct k_timer *timer_id);
void led2_timer_callback(struct k_timer *timer_id);
int main(void)
{
        // configuring GPIOS to become LOW at start
        gpio_pin_configure_dt(&led0_spec, GPIO_OUTPUT_INACTIVE);
        gpio_pin_configure_dt(&led1_spec, GPIO_OUTPUT_INACTIVE);
        gpio_pin_configure_dt(&led2_spec, GPIO_OUTPUT_INACTIVE);

        // initializing all the software timers with respective handles
        k_timer_init(&led0_timer, led0_timer_callback, NULL); // last parameter is NULL because we know the timers will run forever. So no need to mention
                                                              // a stop function that will execute when the timer stopped running
        k_timer_init(&led1_timer, led1_timer_callback, NULL);
        k_timer_init(&led2_timer, led2_timer_callback, NULL);
        // start the timers
        k_timer_start(&led0_timer, K_NO_WAIT, K_MSEC(500)); // 1st parameter is handle of the timer that needs to be started
                                                            // 2nd parameter is how much time it needs to wait before starting the timer (initial delay)
                                                            // 3rd parameter is what is the period between each callback execution.
        k_timer_start(&led1_timer, K_NO_WAIT, K_MSEC(250));
        k_timer_start(&led2_timer, K_NO_WAIT, K_MSEC(100));
        return 0;
}

void led0_timer_callback(struct k_timer *timer_id)
{
        gpio_pin_toggle_dt(&led0_spec); // we are just toggling the LED here
}

void led1_timer_callback(struct k_timer *timer_id)
{
        gpio_pin_toggle_dt(&led1_spec);
}

void led2_timer_callback(struct k_timer *timer_id)
{
        gpio_pin_toggle_dt(&led2_spec);
}