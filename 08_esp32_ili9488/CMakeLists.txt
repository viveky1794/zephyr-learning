cmake_minimum_required(VERSION 3.20.0)

# Add this line so we do not need to pass the board name again and again during build
set(BOARD doit_esp32_devkit_v1/esp32/procpu)

find_package(Zephyr)
project(esp32_spi_display)
target_sources(app PRIVATE src/main.c)