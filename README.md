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
	
# Build

```
west build -b stm32f4_disco -p always
```

# Flash

```
west flash -r openocd
```
