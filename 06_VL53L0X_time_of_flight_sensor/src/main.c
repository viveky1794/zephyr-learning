#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>
#include <string.h>

#define SENSOR_NODE DT_NODELABEL(vl53l0x)

#if !DT_NODE_HAS_STATUS(SENSOR_NODE, okay)
#error "VL53L0X node not enabled in DeviceTree"
#endif

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

    const struct device *dev =  DEVICE_DT_GET(SENSOR_NODE) ;

    /*---- DT_BUS macro can give the bus type which is used by the peripherial ----*/
    static const struct device *i2c_dev = DEVICE_DT_GET(DT_BUS(SENSOR_NODE));

    /*---- Just to check the i2c device address ----*/
    if (!device_is_ready(i2c_dev)) {
        printk("I2C not ready\n");
        return 0;
    }

    i2c_scan_bus(i2c_dev);

    /*---- Check actual sensor is ready or not ----*/
    if (!device_is_ready(dev)) {
        printk("Device is not ready\n");
        return 0;
    }

    printk("VL53L0X ready\n");

    while (1) {
        /*---- For these API look @zephyr/include/zephyr/drivers/sensor.h ----*/
        if (sensor_sample_fetch(dev) < 0) {
            printk("Sample fetch error\n");
            continue;          
        }
        struct sensor_value distance;

        if (sensor_channel_get(dev,
                               SENSOR_CHAN_DISTANCE,
                               &distance) < 0) {
            printk("Channel get error\n");
            continue;
        }

        printk("Distance: %d.%06d m\n",
               distance.val1,
               distance.val2);      
    }
	
	return 0;
}




