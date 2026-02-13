#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define EXT_LED_NODE DT_ALIAS(extled)

static const struct gpio_dt_spec ext_led =
        GPIO_DT_SPEC_GET(EXT_LED_NODE, gpios);

int main(void)
{
    if (!gpio_is_ready_dt(&ext_led)) {
        printk("GPIO not ready\n");
        return 0;
    }

    gpio_pin_configure_dt(&ext_led, GPIO_OUTPUT_INACTIVE);

    printk("External LED blink start\n");

    while (1) {
        gpio_pin_toggle_dt(&ext_led);
        printk("LED toggled\n");
        k_msleep(500);
    }
}

