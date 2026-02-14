
1. Catch configuration errors at build time NOT at runtime.
DEVICE_DT_GET() gives your app a pointer to a hardware device defined in Devicetree. If that device is not defined, disabled, or its driver is not enabled, the build fails with a linker error — so mistakes are caught early.

2. Find your board Device tree file at location **zephyr/boards/st/stm32f4_disco/stm32f4_disco.dts**