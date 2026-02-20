# Basic Information

Host OS: Ubuntu 22.04

Board: STM32F407x Discovery

# Project Overview with concepts to Learn
```tree
my_app/
├── boards/
|    └── stm32f4_disco.overlay   
├── CMakeLists.txt
├── prj.conf
├── src/
    └── main.c
``` 
# Project : Interface Time of flight sensor (VL52L0X)

1. Time of flight sensor (VL52L0X), address = 0x29
2. Board I2C instance: i2c2 (PB6==>SCL, PB9==>SDA)

# Build

```
west build -b stm32f4_disco -p always
```

# Flash

```
west flash -r openocd
```
