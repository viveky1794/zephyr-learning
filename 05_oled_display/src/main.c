#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>

/*Get the pointer of device tree node */
#define RTC_NODE DT_NODELABEL(oled_ssd1306)

/* Get the 'reg' property value from the device tree node */
#define RTC_ADDR DT_REG_ADDR(RTC_NODE)

static const struct device *i2c_dev = DEVICE_DT_GET(DT_BUS(RTC_NODE));

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
    if (!device_is_ready(i2c_dev)) {
        printk("I2C device not ready\n");
        return 0;
    }
	
	i2c_scan_bus(i2c_dev);
	
	while(1) {
		
	}
	
   return 0; 
}
