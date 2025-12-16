# DE10-Nano FPGA + Linux LED / IO System (Avalon-MM)

This repository is a complete FPGA + Linux stack for the Terasic **DE10-Nano (Cyclone V SoC)** that exposes simple hardware peripherals (LEDs, pushbutton, ADC, RGB PWM) to Linux via **Avalon-MM** and **custom Linux kernel modules**.

It’s organized to show the full pipeline—from **Quartus/Qsys system integration**, to **VHDL peripheral IP**, to **Device Tree**, to **Linux drivers**, to **userspace demo scripts/programs**.

## What this does

- Drives the DE10-Nano user LEDs through an Avalon-MM peripheral
- Reads a pushbutton through an Avalon-MM peripheral
- Generates **RGB LED PWM** in FPGA fabric with memory-mapped control registers
- Exposes peripherals to Linux with custom kernel modules:
  - ADC driver (`linux/adc/`)
  - LED bar driver (`linux/led_bar/`)
  - Pushbutton driver (`linux/push_button/`)
  - RGB PWM driver (`linux/rgb_pwm/`)
- Includes userspace demos (shell + C) to exercise the drivers

## Repository layout

- [`docs/`](docs/) — project documentation (overview, architecture, workflow, setup, troubleshooting, final report)
- [`hdl/`](hdl/) — VHDL peripheral IP blocks  
  - [`hdl/led-bar/`](hdl/led-bar/) — LED bus Avalon-MM peripheral ([`ledbus_avalon.vhd`](hdl/led-bar/ledbus_avalon.vhd))  
  - [`hdl/push-button/`](hdl/push-button/) — pushbutton Avalon-MM peripheral ([`push_button_avalon.vhd`](hdl/push-button/push_button_avalon.vhd), [`push_button_avalon_hw.tcl`](hdl/push-button/push_button_avalon_hw.tcl))  
  - [`hdl/rgb_led/`](hdl/rgb_led/) — RGB PWM building blocks + Avalon wrapper ([`pwm_controller.vhd`](hdl/rgb_led/pwm_controller.vhd), [`pwm_rgb.vhd`](hdl/rgb_led/pwm_rgb.vhd), [`pwm_rgb_avalon.vhd`](hdl/rgb_led/pwm_rgb_avalon.vhd))
- [`linux/`](linux/) — Linux kernel drivers + build files (one folder per module)  
  - [`linux/dts/`](linux/dts/) — Device Tree ([`socfpga_cyclone5_de10nano_final_project.dts`](linux/dts/socfpga_cyclone5_de10nano_final_project.dts))
- [`quartus/`](quartus/) — Quartus project + Qsys system ([`soc_system.qsys`](quartus/soc_system.qsys))
- [`sw/`](sw/) — userspace demos/utilities (shell scripts + C helpers like [`pot_to_rgb.c`](sw/pot_to_rgb.c))
- [`utils/`](utils/) — helper scripts + Makefile tooling (ex: [`arm_env.sh`](utils/arm_env.sh))

## How the system fits together (high level)

1. **Quartus/Qsys** integrates the SoC system ([`quartus/soc_system.qsys`](quartus/soc_system.qsys)) and exposes Avalon-MM address space to the HPS.
2. **VHDL IP** in [`hdl/`](hdl/) implements the peripherals (LED bus, pushbutton, RGB PWM).
3. **Device Tree** in [`linux/dts/`](linux/dts/) describes the memory-mapped peripherals so Linux can bind drivers.
4. **Kernel modules** in [`linux/`](linux/) map the Avalon registers and expose interfaces to userspace.
5. **Userspace demos** in [`sw/`](sw/) read/write those interfaces to show the system working.

## Demos

- RGB PWM demo script: [`linux/rgb_pwm/rgb_demo.sh`](linux/rgb_pwm/rgb_demo.sh)
- Userspace control examples: see [`sw/`](sw/) (scripts + [`pot_to_rgb.c`](sw/pot_to_rgb.c))

## Skills demonstrated

- FPGA design in VHDL (PWM generation, Avalon-MM register interfaces, clean module boundaries)
- Avalon-MM peripheral design and SoC integration (Qsys/Platform Designer)
- Linux kernel driver development (platform drivers, MMIO, sysfs/char interfaces depending on module)
- Device Tree integration for custom hardware
- Practical repo organization + documentation aimed at maintainability

## More documentation

- [Overview](docs/overview.md)
- [Architecture](docs/architecture.md)
- [Setup](docs/setup.md)
- [Usage](docs/usage.md)
- [Workflow](docs/workflow.md)
- [Troubleshooting](docs/troubleshooting.md)
- [Final Report](docs/final-report.md)
