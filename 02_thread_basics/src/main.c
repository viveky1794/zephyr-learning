#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* Get LED nodes from devicetree */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

/* Thread stack sizes */
#define STACK_SIZE 512
#define PRIORITY   5

K_THREAD_STACK_DEFINE(stack1, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack2, STACK_SIZE);

struct k_thread thread1_data;
struct k_thread thread2_data;

/* ---------------- Thread 1 ---------------- */
void led_thread1(void *a, void *b, void *c)
{
    while (1) {
        gpio_pin_toggle_dt(&led0);
        printk("Thread1: LED0 toggle\n");
        k_msleep(500);
    }
}

/* ---------------- Thread 2 ---------------- */
void led_thread2(void *a, void *b, void *c)
{
    while (1) {
        gpio_pin_toggle_dt(&led1);
        printk("Thread2: LED1 toggle\n");
        k_msleep(900);
    }
}

/* ---------------- Main ---------------- */
int main(void)
{
    if (!gpio_is_ready_dt(&led0) || !gpio_is_ready_dt(&led1)) {
        printk("LED device not ready\n");
        return 0;
    }

    gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);

    k_thread_create(&thread1_data, stack1, STACK_SIZE,
                    led_thread1, NULL, NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&thread2_data, stack2, STACK_SIZE,
                    led_thread2, NULL, NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);

    printk("Main started — threads created\n");
    return 0;
}

