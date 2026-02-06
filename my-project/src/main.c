#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>

/* Get LED node from devicetree alias */
#define LED_NODE DT_ALIAS(led0)

#if !DT_NODE_HAS_STATUS(LED_NODE, okay)
#error "Board does not define led0 alias"
#endif

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

int main(void)
{
    int ret;

    if (!gpio_is_ready_dt(&led)) {
        return 0;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return 0;
    }

    while (1) {
        gpio_pin_toggle_dt(&led);
        k_msleep(500);
    }
}

