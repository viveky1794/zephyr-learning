# Basic Information

Host OS: Ubuntu 22.04

Board: STM32F407x Discovery

# Project Overview with concepts to Learn
```tree
ext_led_app/
├── CMakeLists.txt
├── prj.conf
├── src/
│   └── main.c
└── boards/
    └── stm32f4_disco.overlay
``` 

Here I use GPIO Port-A[2] for controlling my external Led using overlay concept[1].

Tip-1=> 

- Overlay file must be inside boards/ and named exactly: <board_name>.overlay
- Zephyr build system auto-detects it.
- The build system combines the board’s .dts file and any .overlay files by concatenating them, with the overlays put last.
- The contents of the .overlay file have priority over any definitions in the board’s .dts file or its includes.
- The precedence is always visible in the build output, where Zephr lists all overlay files using the output Found devicetree overlay:
- Overlay files can have multiple places and they have different prededence.

```c
-- Found Dtc: /home/vivek/zephyr-sdk-0.17.4/sysroots/x86_64-pokysdk-linux/usr/bin/dtc (found suitable version "1.7.0", minimum required is "1.4.6") 
-- Found BOARD.dts: /home/vivek/zephyrproject/zephyr/boards/st/stm32f4_disco/stm32f4_disco.dts
-- Found devicetree overlay: /home/vivek/workspace/zephyer-ws/zephyrTestProject/zephyr-learning/03_external_pin/boards/stm32f4_disco.overlay
-- Generated zephyr.dts: /home/vivek/workspace/zephyer-ws/zephyrTestProject/zephyr-learning/03_external_pin/build/zephyr/zephyr.dts
-- Generated pickled edt: /home/vivek/workspace/zephyer-ws/zephyrTestProject/zephyr-learning/03_external_pin/build/zephyr/edt.pickle
-- Generated devicetree_generated.h: /home/vivek/workspace/zephyer-ws/zephyrTestProject/zephyr-learning/03_external_pin/build/zephyr/include/	 generated/zephyr/devicetree_generated.h
```

## Overlays by Example
Let’s try and visualize this list using an imaginary file tree and board “dummy_board”. I’ve annotated the files with precedence numbers, even though - as we’ve learned before - not all files will be used by the build:
```c
$ tree --charset=utf-8 --dirsfirst.
.
├── boards
│   ├── dummy_board_123.overlay -- #3
│   └── dummy_board.overlay     -- #4
├── dts
│   └── extra
│       ├── extra_0.overlay -- #2
│       └── extra_1.overlay -- #1
├── src
│   └── main.c
├── CMakeLists.txt
├── app.overlay         -- #6
├── dummy_board.overlay -- #5
└── prj.conf
```
------
Let’s assume we would use the following command that doesn’t include the CMake variable DTC_OVERLAY_FILE:
```c
$ west build --no-sysbuild --board dummy_board@123 -- \
  -DEXTRA_DTC_OVERLAY_FILE="dts/extra/extra_0.overlay;dts/extra/extra_1.overlay"
```
The overlay files would be detected and added as follows, depending on whether or not they exist. As mentioned, the precedence increases and thus the files listed last have the highest precedence:


    - boards/dummy_board.overlay
    - boards/dummy_board_123.overlay
    - if none of the previous files exist, dummy_board.overlay
    - if none of the previous files exist, app.overlay
    - dts/extra/extra_0.overlay
    - dts/extra/extra_1.overlay

**Note:** It is recommended to use the boards directory for board overlay files. You should no longer place your board’s overlay files in the application’s root directory.


If, instead, we specify the CMake variable DTC_OVERLAY_FILE as app.overlay in our command as follows, the automatic detection is skipped and the build process only picks the selected DTC overlay files:

```c
$ west build --no-sysbuild --board dummy_board@123 -- \
  -DTC_OVERLAY_FILE="app.overlay" \
  -DEXTRA_DTC_OVERLAY_FILE="dts/extra/extra_0.overlay;dts/extra/extra_1.overlay"
```

The overlay files would thus be added as follows and with increasing precedence:

    - app.overlay
    - dts/extra/extra_0.overlay
    - dts/extra/extra_1.overlay

## Understanding Devicetree macro names

In Zephyr’s doc folder, you can find the “RFC 7405 ABNF grammar for Devicetree macros” zephyr/doc/build/dts/macros.bnf. This RFC describes the macros that are directly generated out of the Devicetree sources.

    - DT is the common prefix for Devicetree macros,
    - S is a forward slash /,
    - N refers to a node,
    - P is a property,
    - all letters are converted to lowercase,
    - and non-alphanumerics characters are converted to underscores “_”

```c
zephyr/dts/arm/nordic/nrf52840.dtsi

/ {
  soc {
    uart0: uart@40002000 {
      compatible = "nordic,nrf-uarte";
      reg = <0x40002000 0x1000>;
    };
  };
};

zephyr/boards/nordic/nrf52840dk/nrf52840dk_nrf52840.dts

&uart0 {
  compatible = "nordic,nrf-uarte";
  status = "okay";
  current-speed = <115200>;
  /* other properties */
};
```
Following the previous rules, we can transform the paths and property names into tokens:

    - The node path /soc/uart@40002000 is transformed to _S_soc_S_uart_40002000.
    - The property name current-speed is transformed to current_speed.
    - The property name status stays the same.

Since node paths are unique, by combining the node’s path with its property names we can create a unique macro for each property.

    - /soc/uart@40002000, property current-speed becomes N_S_soc_S_uart_40002000_P_current_speed
    - /soc/uart@40002000, property status becomes N_S_soc_S_uart_40002000_P_status




Tip-2=> Make changes in <board_name>.overlay file by looking into board file.
	DTS File Path: 
		- zephyrproject/zephyr/boards/st/stm32f4_disco/stm32f4_disco.dts
		- dts/arm/st/f4/stm32f4.dtsi
			
## Matching compatible bindings
`compatible` is a list of strings that identifies which device model(s) a Devicetree node represents. Each string maps to a `binding`.

A `binding` defines how a specific device type must be described in Devicetree — including which properties are allowed or required, their data types, and their meaning.

Without a binding, a node could contain arbitrary properties and values (as long as syntax is valid), and their names would have no standardized interpretation. Bindings bring structure and validation.

From the Devicetree specification:

- `compatible` is a standard property
- Type: `string list`
- Purpose: declare compatibility with a device or device family
- Recommended format: "manufacturer,model"
- Format rules: lowercase letters, digits, and dashes only; must start with a letter.
				A single comma is typically only used following a vendor prefix.
	
Providing a compatible binding tells the Devicetree tooling to validate that a node conforms to a defined device schema — meaning its properties, required fields, and data types are checked against the binding rules. It also tells the build system how to interpret that node and which driver model it should map to. In short, bindings both enforce correctness and attach semantic meaning to the node’s properties so they can be used reliably by drivers and generated code.

Bindings live outside the Devicetree. The DTSpec doesn’t specify any file format or syntax that is used for bindings. Zephyr, like Linux, uses .yaml files for its bindings. Zephyr recursively looks for Devicetree bindings (.yaml files) in zephyr/dts/bindings. Bindings are matched against the strings provided in the compatible property of a node. Conveniently, binding files in Zephyr use the same basename as the compatible string

If a node lists multiple compatible strings, the build system selects the first string that matches a binding and uses that driver — for example, compatible = "st,stm32f407-gpio", "st,stm32-gpio"; will bind to the F407-specific driver if available, otherwise fall back to the generic STM32 GPIO driver.

File need to visit once for better understanding.

- This is Zephyr’s base model for UART controllers, which is used regardless of the actual vendor:
	File: zephyr/dts/bindings/serial/uart-controller.yaml

- This binding contains common fields used by all devices in Zephyr. Here, we do not only encounter the status property but also compatible:
	File: zephyr/dts/bindings/base/base.yaml

**Note:** n contrast to overlays, however, detected bindings are not listed in the build output.


# Build

```
west build -b stm32f4_disco -p always
```

# Flash

```
west flash -r openocd
```
