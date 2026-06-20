#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/display.h>
#include <zephyr/sys/printk.h>


#define OLED_NODE DT_NODELABEL(oled_ssd1306)

void i2c_scan_bus(const struct device *i2c_dev)
{
    int ret;
    uint8_t addr = 0;
    printk("Scanning I2C bus...\n");

    for (addr = 0x00; addr <= 0xFE; addr++) {
        /* zero-length write = address probe */
        ret = i2c_write(i2c_dev, NULL, 0, addr);
        if (ret == 0) {
            printk("Found device at 0x%02X\n", addr);
        }
    }

    printk("Scan done\n");
}

int main(void)
{
    /*---- Get the pointer of device tree node ----*/
    const struct device *oled_dev = DEVICE_DT_GET(OLED_NODE);

    /*---- Get the pointer of bus used by device node ----*/
    const struct device *i2c_dev = DEVICE_DT_GET(DT_BUS(OLED_NODE));
    if (!device_is_ready(i2c_dev)) {
        printk("I2C device not ready\n");
        return 0;
    }
	
	i2c_scan_bus(i2c_dev);

    device_init(oled_dev);
    if (!device_is_ready(oled_dev)) {
        printk("ERROR: OLED device not ready\n");
        return 0;
    }

    printk("OLED device is ready\n");
    display_blanking_off(oled_dev);

    #define WIDTH   128
    #define HEIGHT   64
    #define BUF_SIZE (WIDTH * HEIGHT / 8)

    uint8_t buf[BUF_SIZE];

    struct display_buffer_descriptor desc = {
        .width = WIDTH,
        .height = HEIGHT,
        .pitch = WIDTH,
        .buf_size = BUF_SIZE,
    };

    while (1) {

        /* 1. All black */
        memset(buf, 0x00, BUF_SIZE);
        display_write(oled_dev, 0, 0, &desc, &buf);
        k_sleep(K_SECONDS(1));

        /* 2. All white */
        memset(buf, 0xFF, BUF_SIZE);
        display_write(oled_dev, 0, 0, &desc, buf);
        k_sleep(K_SECONDS(1));

        /* 3. Vertical stripes */
        for (int i = 0; i < BUF_SIZE; i++) {
            buf[i] = (i % 2) ? 0xFF : 0x00;
        }
        display_write(oled_dev, 0, 0, &desc, buf);
        k_sleep(K_SECONDS(1));

        /* 4. Checkerboard */
        for (int i = 0; i < BUF_SIZE; i++) {
            buf[i] = (i / WIDTH) % 2 ? 0xAA : 0x55;
        }
        display_write(oled_dev, 0, 0, &desc, buf);
        k_sleep(K_SECONDS(1));
    }
   return 0; 
}
