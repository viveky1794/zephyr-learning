# Basic Information

Host OS: Ubuntu 22.04

Board: STM32F407x Discovery

# Project Overview with concepts to Learn
```tree
my_app/
├── CMakeLists.txt
├── prj.conf
├── src/
    └── main.c
``` 
# Project : Interface OLED display (SSD1306)

1. OLED display (SSD1306)
2. Board I2C instance: i2c2 (PB10 ==>SCL, PB11 ==>SDA)
3. Getting curruption and sysmpthons are as below.
Resolve this error why incrase the size of stack.

```c
*** Booting Zephyr OS build v4.3.0-5193-g4f3478391a3c ***
Scanning I2C bus...
Found device at 0x3C
Found device at 0xBC
Scan done
[00:00:00.042,000] <err> os: ***** USAGE FAULT *****
[00:00:00.042,000] <err> os:   Illegal use of the EPSR
[00:00:00.042,000] <err> os: r0/a1:  0x00000004  r1/a2:  0x00000000  r2/a3:  0x00000005
[00:00:00.042,000] <err> os: r3/a4:  0x200000b8 r12/ip:  0x0000002a r14/lr:  0x00000000
[00:00:00.042,000] <err> os:  xpsr:  0x80000000
[00:00:00.042,000] <err> os: Faulting instruction address (r15/pc): 0x00000000
[00:00:00.042,000] <err> os: >>> ZEPHYR FATAL ERROR 35: Unknown error on CPU 0
[00:00:00.042,000] <err> os: Current thread: 0x20000608 (unknown)
[00:00:00.070,000] <err> os: Halting system
```

4. Get Necessary API to use at path `/home/vivek/zephyrproject/zephyr/include/zephyr/drivers/display.h`
5. Get ssd1306 Display driver at path `/home/vivek/zephyrproject/zephyr/drivers/display/display_ssd1306.c`

	
# Build

```
west build -b stm32f4_disco -p always
```

# Flash

```
west flash -r openocd
```
