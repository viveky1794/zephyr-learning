#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>

/*Get the pointer of device tree node */
#define RTC_NODE DT_NODELABEL(ds1307)

/* Get the 'reg' property value from the device tree node */
#define RTC_ADDR DT_REG_ADDR(RTC_NODE)

static const struct device *i2c_dev = DEVICE_DT_GET(DT_BUS(RTC_NODE));

const char weekdays[7][15] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thrusday", "Friday", "Saturday"};

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

#ifdef RTC_CONFIG
int ds1307_set_datetime(const struct device *i2c_dev,
                        int sec, int min, int hour,
                        int day, int date, int month, int year)
{
    uint8_t buf[8];

    buf[0] = 0x00; // start register

    buf[1] = bin2bcd(sec) & 0x7F;   // ensure CH bit = 0
    buf[2] = bin2bcd(min);
    buf[3] = bin2bcd(hour);         // 24-hour mode
    buf[4] = bin2bcd(day);
    buf[5] = bin2bcd(date);
    buf[6] = bin2bcd(month);
    buf[7] = bin2bcd(year);         // last two digits

    return i2c_write(i2c_dev, buf, sizeof(buf), RTC_ADDR);
}
#endif

int main(void)
{
    if (!device_is_ready(i2c_dev)) {
        printk("I2C device not ready\n");
        return 0;
    }

    i2c_scan_bus(i2c_dev);

    printk("DS1307 RTC demo\n");

#ifdef RTC_CONFIG
    /* Set time: 21:30:00 — Friday — 13 Feb 2026 */
    ds1307_set_datetime(i2c_dev,
		    0,   // sec
		    54,  // min
		    19,  // hour
		    2,   // day of week
		    17,  // date
		    2,   // month
		    26); // year (2026 -> 26)
#endif

    while (1) {
        uint8_t reg = 0x00;
        uint8_t buf[7];

        int ret = i2c_write_read(i2c_dev,
                                 RTC_ADDR,
                                 &reg, 1,
                                 buf, 7);

        if (ret == 0) {
            int sec  = bcd2bin(buf[0] & 0x7F);
            int min  = bcd2bin(buf[1]);
            int hour = bcd2bin(buf[2] & 0x3F);
	    int day   = bcd2bin(buf[3]);
	    int date  = bcd2bin(buf[4]);
	    int month = bcd2bin(buf[5]);
	    int year  = bcd2bin(buf[6]);

	    printk("RTC: %02d-%02d-20%02d  %02d:%02d:%02d  DOW:%s\n",
           	date, month, year,
           	hour, min, sec,
           	weekdays[day]);

        } else {
            printk("RTC read error %d\n", ret);
        }

        k_sleep(K_SECONDS(1));
    }
}

