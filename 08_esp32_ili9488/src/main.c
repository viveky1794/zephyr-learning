/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**********************************
| TFT           | Esp32           |
|---------------|-----------------|
| Vcc           | 3v              |
|---------------|-----------------|
| GND           | GND             |
|---------------|-----------------|
| CS            | D15             |
|---------------|-----------------|
| RESET         | D4              |
|---------------|-----------------|
| DC/RS         | D2              |
|---------------|-----------------|
| MOSI(6)       | D23             |
|---------------|-----------------|
| SCK           | D18             |
|---------------|-----------------|
| LED           | D32             |
|---------------|-----------------|
| MISO          |                 |
|---------------|-----------------|
| T_CLK         |                 |
|---------------|-----------------|
| T_CS          |                 |
|---------------|-----------------|
| T_DIN         |                 |
|---------------|-----------------|
| T_DO          |                 |
|---------------|-----------------|
| T_IRQ         |                 |
|---------------|-----------------|
|               |                 |
***********************************/

#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree_generated.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/display.h>
#include "image_320x480_bgr888_from_landscape.h"

/* Backlight GPIO from zephyr,user */
#define USER_NODE DT_PATH(zephyr_user)

#if DT_NODE_HAS_PROP(USER_NODE, bl_gpios)
static const struct gpio_dt_spec bl_gpio =
	GPIO_DT_SPEC_GET(USER_NODE, bl_gpios);
#endif

/* ILI9488 display node */
#define DISPLAY_NODE DT_NODELABEL(ili9488)

static void backlight_on(void)
{
#if DT_NODE_HAS_PROP(USER_NODE, bl_gpios)
	if (!gpio_is_ready_dt(&bl_gpio)) {
		printk("Backlight GPIO not ready\n");
		return;
	}

	gpio_pin_configure_dt(&bl_gpio, GPIO_OUTPUT_ACTIVE);
	gpio_pin_set_dt(&bl_gpio, 1);
#endif
}

int main(void)
{
	const struct device *display_dev;
	struct display_capabilities caps;

	printk("ILI9488 demo start\n");

	display_dev = DEVICE_DT_GET(DISPLAY_NODE);

	if (!device_is_ready(display_dev)) {
		printk("Display device not ready\n");
		return -ENODEV;
	}

	backlight_on();
    
	display_get_capabilities(display_dev, &caps);

	printk("Resolution: %ux%u\n",
	       caps.x_resolution,
	       caps.y_resolution);

	display_blanking_off(display_dev);

	/*
	 * Create a solid red screen.
	 * For RGB888 format:
	 * R = 0xFF
	 * G = 0x00
	 * B = 0x00
	 */
    
     /*framebuf is store into RAM (.bss) region */
	static uint8_t framebuf[320 * 20 * 3];

	for (size_t i = 0; i < sizeof(framebuf); i += 3) {
		framebuf[i + 0] = 0xFF; /* R */
		framebuf[i + 1] = 0x00; /* G */
		framebuf[i + 2] = 0x00; /* B */
	}
   
	struct display_buffer_descriptor desc = {
		.buf_size = sizeof(framebuf),
		.width = 320,
		.height = 20,
		.pitch = 320,
	};
    
	for (uint16_t y = 0; y < 480; y += 20) {
		display_write(display_dev, 0, y,
			      &desc, framebuf);
	}

	printk("Red screen displayed\n");
    
    k_sleep(K_SECONDS(3));

	struct display_buffer_descriptor desc_image = {
		.buf_size = sizeof(image_bgr888),
		.width    = IMAGE_WIDTH,
		.height   = IMAGE_HEIGHT,
		.pitch    = IMAGE_WIDTH,
	};

	display_write(display_dev, 0, 0, &desc_image, image_bgr888);

	while (1) {
		k_sleep(K_SECONDS(1));
	}

	return 0;
}