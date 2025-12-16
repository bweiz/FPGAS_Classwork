<!-- docs/overview.md -->

# Overview

This repository is a full-stack FPGA + Linux project targeting the **Terasic DE10-Nano (Cyclone V SoC)**. It demonstrates an end-to-end workflow:

- Custom **FPGA peripherals** written in **VHDL**
- Integration into a **Platform Designer (Qsys)** system and Quartus build
- Exposure of FPGA registers to the **HPS (ARM/Linux)** via the HPS-to-FPGA bridge
- **Linux platform drivers** (loadable kernel modules) that map FPGA registers and expose clean user-space control paths
- Small **user-space utilities/scripts** to demo the system

The project’s core message is: **hardware IP + SoC integration + Linux driver bring-up + user-space demos**.

## System capabilities

The system provides control and demos for several LED-related peripherals:

- **On-board user LEDs** (register-controlled output)
- **Push button input** (readable status register, debounced in hardware)
- **RGB LED PWM** (period + duty control for R/G/B)
- **External LED bar** (driver + demo scripts)
- **ADC path** (driver + software hooks, depending on wiring/lab setup)

## Repository structure

- [`hdl/`](../hdl/README.md) — VHDL peripherals and Avalon-MM wrappers  
  - `led-bar/` — LED bus Avalon peripheral  
  - `push-button/` — push-button Avalon peripheral (+ component TCL)  
  - `rgb_led/` — PWM core + Avalon wrapper (+ component TCL)
- [`quartus/`](../quartus/README.md) — Quartus project + Platform Designer system
- [`linux/`](../linux/README.md) — kernel modules + device tree source  
  - `dts/` — device tree source for the peripherals
- [`sw/`](../sw/README.md) — user-space demos and helper scripts
- [`docs/`](./README.md) — documentation hub

## Getting started

See:
- [`setup.md`](./setup.md) — build/boot/driver workflow
- [`usage.md`](./usage.md) — run demos and interact with drivers
