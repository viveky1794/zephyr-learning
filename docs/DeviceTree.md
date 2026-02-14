
# Device Tree (Need a separate file)
- The Devicetree is a tree data structure you provide to describe your hardware. Each node describes one device, e.g., the UART.

- Except for the root note, each node has exactly one parent, thus the term devicetree.

- There are also some subtle differences between the official Devicetree specification and the way it is used in Zephyr.

**PATH :** build/zephyr/zephyr.dts

- You might have noticed the file extensions `.dts` and `.dtsi`. Both file extensions are Devicetree source files, but by convention the `.dtsi` extension is used for DTS files that are intended to be included by other files.

- Zephyr feeds the generated build/zephyr/zephyr.dts.pre into its own `GEN_DEFINES_SCRIPT` Python script, located at `zephyr/scripts/dts/gen_defines.py`.

- Zephyr, however, is designed to run on resource-constrained, embedded systems. It is simply not feasible to load a Devicetree blob during runtime: Any such structure would take up too many resources in both the Zephyr drivers and storing the Devicetree blob. Instead, the Devicetree is resolved during compile time.

## Decipher devicetree_generated.h file content
```
#define DT_CHOSEN_zephyr_console DT_N_S_soc_S_uart_40002000
// --snip---
#define DT_N_S_soc_S_uart_40002000_P_current_speed 115200
#define DT_N_S_soc_S_uart_40002000_P_status "okay"
```
Looks cryptic? With just a few hints, this becomes readable. Know that:

    DT_ is just the common prefix for Devicetree macros,
    _S_ is a forward slash /,
    _N_ refers to a node,
    _P_ is a property.

Thus, e.g., DT_N_S_soc_S_uart_40002000_P_current_speed simply refers to the property current_speed of the node /soc/uart_40002000

- Finally, the generated dts.cmake is a file that basically allows to access the entire Devicetree from within CMake, using CMake target properties, e.g., we’ll find the current speed of our UART peripheral also within CMake:

dts.cmake
```dts
set_target_properties(
    devicetree_target
    PROPERTIES
    "DT_PROP|/soc/uart@40002000|current-speed" "115200"
)
```

# Devicetree `/aliases` — Summary (Zephyr / DTSpec)

## What is `/aliases`?

- `/aliases` is a child node of the Devicetree root.
- Each property inside `/aliases` defines an **alias name → full node path** mapping.
- It provides an alternate, stable way to reference nodes in an application.

**DTSpec definition:**
- Property **name** = alias name
- Property **value** = full path to a Devicetree node

---

## How Aliases Are Defined

Aliases can point to nodes using:
- Node label reference
- Explicit node path reference
- Plain string path

Example:

```dts
/ {
  aliases {
    alias-by-label = &label_a;
    alias-by-path = &{/node_a};
    alias-as-string = "/node_a";
  };

  label_a: node_a {
    /* Empty */
  };
};
```

## Aliases vs Labels

Both aliases and labels help reference nodes, but they are not interchangeable.

**Labels**

- Can be referenced inside Devicetree using &label
- Work in DTS phandle references
- Used during Devicetree compilation

**Aliases**

- Only for application-level access
- Cannot be used as phandle references inside DTS
- Compiled as normal node properties

```dts
/ {
  aliases {
    alias-by-label = &label_a;
  };
  label_a: node_a {
    /* Empty. */
  };
  node_ref {
    // This doesn't work. An alias cannot be used like labels.
    phandle-by-alias = <&alias-by-label>;
  };
};
```

# Why Aliases Matter (Portability Across Boards)

Different boards often use:

- Different node paths  
- Different labels  
- Different naming conventions  

This makes direct Devicetree references non-portable across boards.

---

## Example — Button Nodes

### Nordic nRF52840 DK

```dts
zephyr/boards/nordic/nrf52840dk/nrf52840dk_nrf52840.dts

/ {
  buttons {
    compatible = "gpio-keys";
    button0: button_0 { /* ... */ };
    button1: button_1 { /* ... */ };
    button2: button_2 { /* ... */ };
    button3: button_3 { /* ... */ };
  };
};
```

### STM32F4_disco Board
```dts
zephyr/boards/st/nucleo_c031c6/nucleo_c031c6.dts

/ {
  gpio_keys {
    compatible = "gpio-keys";
    user_button: button { /* ... */ };
  };
};
```

-So, our STM board has only one button, which has the path /gpio_keys/button and the label user_button - both incompatible with the references we’d use for our nRF development kit.
However, if we’d like to use this button in our application, we’d now have to change our sources - or even worse - adapt the DTS files.
Instead of doing this, we can use aliases - and since buttons are commonly used throughout Zephyr’s example applications, the corresponding alias, in our case sw0, already exists:
```dts
zephyr/boards/nordic/nrf52840dk/nrf52840dk_nrf52840.dts

/ {
  aliases {
    led0 = &led0;
    /* ... */
    pwm-led0 = &pwm_led0;
    sw0 = &button0;
    /* ... */
  };
};

zephyr/boards/st/nucleo_c031c6/nucleo_c031c6.dts

/ {
  aliases {
    led0 = &green_led_4;
    pwm-led0 = &green_pwm_led;
    sw0 = &user_button;
    /* ... */
  };
};
```

# Devicetree `/chosen` — Summary (Zephyr)

## What is `/chosen`?

- `/chosen` is a child node of the Devicetree root.
- It does **not represent a real device**.
- It stores **system-level parameters** selected for the build.
- Entries are typically references to other nodes.
- In Zephyr, `/chosen` is used **only at build time**, not at runtime.

---

## Example — `/chosen` in a Zephyr Board DTS

```dts
/ {
  chosen {
    zephyr,console = &uart0;
    zephyr,shell-uart = &uart0;
    zephyr,flash = &flash0;
    zephyr,sram = &sram0;
    zephyr,code-partition = &slot0_partition;
  };
};
```

These properties tell Zephyr which devices to use for:

- Console output
- Shell interface
- Flash memory
- SRAM
- Code partition
- Other system services

# `/chosen` vs `/aliases` — Quick Summary (Zephyr Devicetree)

## Use `/chosen` When

Use `/chosen` to define **system-level selections** used by Zephyr core and subsystems.

Typical purposes:

- Setting global system roles
- Selecting system-wide devices
- Configuring Zephyr infrastructure
- Supplying devices required by kernel/subsystems

Examples:

- Console UART
- Shell UART
- Flash device
- SRAM region

---

## Use `/aliases` When

Use `/aliases` to provide **portable, friendly names** for devices used by applications.

Typical purposes:

- Providing portable application references
- Mapping friendly names to nodes
- Supporting cross-board compatibility
- Simplifying application Devicetree access

---

## Design Guidance

- `/chosen` → system configuration hooks
- `/aliases` → portable naming for applications
- Do not add custom `/chosen` entries for normal app settings
- Prefer **Kconfig** for feature and configuration selection when suitable

