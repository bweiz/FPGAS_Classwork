# RGB PWM Driver (`rgb_pwm.c`)

Platform driver for the **RGB PWM Avalon subsystem** (`pwm_rgb_avalon.vhd`.  
Lets Linux control RGB LED brightness and PWM period via sysfs and a misc char device.

## Building

## Building

The Makefile in this directory cross-compiles the driver. Update the `KDIR` variable to point to your linux-socfpga repository directory.

Run `make` in this directory to build to kernel module.

## Device Tree Node

rgb_pwm: rgb_pwm@ff37f430 {
    compatible = "weizenegger,rgb-pwm";
    reg = <0xff37f430 0x10>;
};

| Offset | Name   | Purpose                              |
| ------ | ------ | ------------------------------------ |
| 0x00   | RED    | Red duty (18.17 fixed, low 18 bits)  |
| 0x04   | GREEN  | Green duty                           |
| 0x08   | BLUE   | Blue duty                            |
| 0x0C   | PERIOD | PWM period (11.5 fixed, low 11 bits) |

The driver reads/writes full 32-bit words; upper bits are ignored by HDL.

## Example

### Purplish Color

echo 80000 | sudo tee /sys/devices/platform/rgb_pwm/red
echo 20000 | sudo tee /sys/devices/platform/rgb_pwm/blue
echo 0     | sudo tee /sys/devices/platform/rgb_pwm/green
echo 1024  | sudo tee /sys/devices/platform/rgb_pwm/period