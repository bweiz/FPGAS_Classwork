# usage.md

# Usage

This guide shows common runtime actions once Linux is booted with the matching FPGA image and DTB.

## 1) Load drivers

From the directory containing the kernel modules:

    sudo insmod rgb_pwm.ko
    sudo insmod push_button.ko
    sudo insmod led_bar.ko
    sudo insmod de10nano_adc.ko
    dmesg | tail -n 50

To unload:

    sudo rmmod rgb_pwm
    sudo rmmod push_button
    sudo rmmod led_bar
    sudo rmmod de10nano_adc

## 2) Locate sysfs nodes

Many drivers expose register access via sysfs:

    ls /sys/devices/platform/

Navigate into the device directory corresponding to the DT node. Attributes typically appear as readable/writable files.

Read:

    cat <attribute>

Write:

    echo <value> > <attribute>

## 3) RGB PWM (example)

Example control sequence (attribute names may vary depending on driver implementation):

    echo 50000 > period
    echo 10000 > red
    echo 0 > green
    echo 0 > blue

## 4) Push button (example)

    cat button

## 5) User-space demos

User-space demos live in `sw/`.

Common entry points:

- `sw/launch.sh`
- `sw/custom_pb_colors.sh`
- `sw/update_led_bar.sh`
- `sw/pot_to_rgb.c`

Example:

    cd sw
    chmod +x *.sh
    ./launch.sh

## Troubleshooting

Useful checks:

    dmesg | tail -n 100
    lsmod | grep -E "rgb|push|led|adc"
    ls /sys/devices/platform/

Common causes of failure:

- FPGA image and DTB mismatch
- `compatible` mismatch between DT and driver
- base address/span mismatch with Platform Designer
