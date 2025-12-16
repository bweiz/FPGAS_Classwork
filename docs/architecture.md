<!-- docs/architecture.md -->

# Architecture

This project uses the **Cyclone V SoC** architecture: FPGA fabric + ARM HPS running Linux.

## Data/control flow

**User space**
- Shell scripts and small C utilities in `sw/`

⬇

**Linux kernel space**
- Platform drivers in `linux/*/` that:
  - map FPGA register space (memory-mapped I/O)
  - expose control via sysfs attributes and/or a `/dev/*` misc character device

⬇

**HPS ↔ FPGA bridge**
- The HPS lightweight bridge (H2F LW) provides register access from ARM to FPGA peripherals

⬇

**FPGA fabric**
- Platform Designer system (`quartus/soc_system.qsys`)
- Custom Avalon-MM slaves:
  - LED bus output
  - Push button input
  - RGB PWM controller registers
  - Optional peripherals depending on wiring/lab setup

## Hardware-to-software integration

1. A VHDL peripheral exposes a set of 32-bit registers through an Avalon-MM slave.
2. Platform Designer assigns the peripheral a base address in the lightweight bridge address map.
3. The device tree declares a node describing the peripheral:
   - `compatible = "<vendor>,<device>";`
   - `reg = <BASE SPAN>;`
4. The Linux driver matches on the `compatible` string, ioremaps the register region, and exposes a user-space API.

## Hardware modules

- [`hdl/led-bar/`](../hdl/led-bar/README.md)
- [`hdl/push-button/`](../hdl/push-button/README.md)
- [`hdl/rgb_led/`](../hdl/rgb_led/README.md)

## Linux drivers

- [`linux/led_bar/`](../linux/led_bar/README.md)
- [`linux/push_button/`](../linux/push_button/README.md)
- [`linux/rgb_pwm/`](../linux/rgb_pwm/README.md)
- [`linux/adc/`](../linux/adc/README.md)

Common driver pattern:
- `platform_driver` with an `of_match_table`
- register mapping using platform resources (ioremap)
- sysfs attributes for register access (and optionally a misc char device)

## Quartus / Platform Designer

The Platform Designer system (`soc_system.qsys`) is generated into HDL and included in the Quartus project under `quartus/`. The Quartus build produces the FPGA image used during bring-up.
z