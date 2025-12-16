# Setup

This guide describes the standard workflow from HDL to a running Linux-controlled system.

## Workflow summary

1. Update HDL (VHDL)
2. Update Platform Designer (Qsys) system and regenerate outputs
3. Build the Quartus project
4. Update the device tree and build the DTB
5. Boot Linux with a matching FPGA image and DTB
6. Build and load kernel modules
7. Run user-space demos

## Prerequisites

- Quartus Prime + Platform Designer (Qsys)
- Terasic DE10-Nano (Cyclone V SoC)
- Linux development environment used in the course (VM or workstation)
- Kernel / device-tree build tooling used in the course
- Kernel module build environment matching the target kernel

## 1) HDL: peripherals

Hardware modules live in `hdl/`:

- `hdl/led-bar/ledbus_avalon.vhd`
- `hdl/push-button/push_button_avalon.vhd`
- `hdl/rgb_led/pwm_rgb_avalon.vhd` (plus `pwm_rgb.vhd`, `pwm_controller.vhd`)

## 2) Platform Designer (Qsys)

In Platform Designer:

- Import/update custom components (TCL + HDL)
- Connect Avalon-MM slaves to:
  - `jtag_master` (debug access)
  - HPS `h2f_lw_axi_master` (Linux access)
- Assign base addresses for each peripheral
- Regenerate HDL outputs

## 3) Quartus build

Quartus project files live in `quartus/`.

- Ensure generated `soc_system` outputs are included
- Compile the design
- Produce the FPGA programming image used by the boot flow

## 4) Device tree

Device tree sources live in:

- `linux/dts/socfpga_cyclone5_de10nano_final_project.dts`

Each FPGA peripheral should have a node with:

- `compatible` string matching the Linux driver
- `reg` matching the base address/span assigned in Platform Designer

After edits:

- build the DTB using the course/kernel workflow
- deploy the DTB to the target boot location

## 5) Boot consistency requirement

The FPGA image and DTB must match the same Platform Designer address map. If the DTB and FPGA image are out of sync, drivers may fail to probe or may read/write incorrect registers.

## 6) Build kernel modules

Kernel module sources live in:

- `linux/adc/`
- `linux/led_bar/`
- `linux/push_button/`
- `linux/rgb_pwm/`

Typical build pattern (example for one driver):

    cd linux/rgb_pwm
    make ARCH=arm

Repeat for other driver directories as needed, then deploy the `.ko` files to the target.

## 7) Load modules on target

Example module load sequence:

    sudo insmod rgb_pwm.ko
    sudo insmod push_button.ko
    sudo insmod led_bar.ko
    sudo insmod de10nano_adc.ko
    dmesg | tail -n 50

If module loading fails, check:

- device tree `compatible` strings
- base address/span mismatches
- FPGA image vs DTB mismatch
- probe errors in `dmesg`

---