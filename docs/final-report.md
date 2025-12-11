# Final Project Report - Team SDC - Ben Weizenegger, Mark Whitney

## System Overview

Our final project implements a small embedded system on the DE10-Nano that connects custom FPGA hardware to Linux on the HPS.

Main pieces:

- **Custom FPGA components**
  - Push Button Interface (synchronizer + debouncer + one-pulse generator)
  - LED Bar Controller (memory-mapped ledbar)
- **Supporting FPGA component**
  - RGB PWM block (`pwm_controller.vhd`, `pwm_rgb.vhd`, `pwm_rgb_avalon.vhd`), which drives the external RGB LED with independent duty cycles per color.
- **HPS / Linux side**
  - Platform drivers that expose each component via sysfs:
    - `/sys/devices/platform/ff37f470.pushbutton/push_button_reg`
    - `/sys/devices/platform/ff37f450.ledbar/sw_led_control`
    - `/sys/bus/platform/devices/ff37f430.rgb_pwm/*`
  - User-space programs and scripts:
    - `pot_to_rgb.c` – reads ADC channels and writes fixed-point RGB duty cycles to the PWM sysfs interface.
    - `custom_pb_colors.sh` – watches the push button and cycles a mode index (0–3) into `/home/soc/number.txt`.
    - `update_led_bar.sh` – writes a count of running linux processes to the LED bar sysfs register.
    - `launch.sh` – starts/stops the demo processes together or separate.

High-level behavior:

- The push button is cleaned up in hardware and exposed as a stable register.
- The C program and shell scripts use sysfs nodes to:
  - Detect button presses,
  - Map potentiometer values to RGB duty cycles,
  - Update the LED bar pattern.

---

## Push Button Interface

The Push Button Interface is a custom Avalon-MM slave that converts a noisy mechanical button into a clean digital event for software.

### Hardware

- **Synchronizer**: brings the raw button signal into the FPGA clock domain (two flip-flops).
- **Debouncer**: counter-based filter that requires the input to be stable for a fixed number of cycles before changing the debounced level.
- **One-pulse logic**: generates a single-cycle pulse on each valid rising edge, so each physical press is counted once.

The interface is exported as a small register, for example:

- `push_button_reg` (at `/sys/devices/platform/ff37f470.pushbutton/push_button_reg`)
  - `0` → no event
  - nonzero → button press detected (cleared when software writes `0`)

### Software Usage

`custom_pb_colors.sh` implements a simple state machine on top of this:

- Polls `push_button_reg` until it becomes nonzero.
- Increments a mode counter `number = (number + 1) % 4`.
- Writes the mode to `/home/soc/number.txt`.
- Writes `0` back to `push_button_reg` to clear the event.

This demonstrates a full path: push button → custom HDL → Avalon → driver → sysfs → shell script.

---

## LED Bar Controller

The LED Bar Controller is a custom Avalon-MM slave that drives an external LED bar using a single memory-mapped register.

### Hardware

- Internal register holds the bar pattern.
- Output bits are directly connected to the 10 LED pins.
- Reset logic clears all LEDs on reset.

Register:

- `sw_led_control` (at `/sys/devices/platform/ff37f450.ledbar/sw_led_control`)
  - Bits `[9:0]` correspond to the 10 LEDs (1 = LED on).

### Software Usage

The `update_led_bar.sh` script shows a simple software-driven usage:

- Reads the current process count with `ps -e --no-headers | wc -l`.
- Echoes that integer into `sw_led_control` in a loop.

This makes the LED bar act as a rough system activity indicator.

The LED bar can also be driven from the C program or tied to the same mode value used for RGB color, so the bar visualizes the current mode or level.

---

## Conclusion

This project tied together:

- Custom HDL (push button interface and LED bar controller, plus an RGB PWM block).
- Avalon-MM integration and address mapping.
- Linux drivers that expose each block as simple sysfs files.
- User-space C and shell scripts that react to hardware inputs and update outputs in real time.

Pressing the button and adjusting the pots immediately changes what you see on the RGB LED and LED bar, which is a clear demonstration of the full hardware–software stack.

The project was useful because it tied the whole class together: FPGA design, Platform Designer, device trees, drivers, and user-space code.

---

## Suggestions

- Provide a reference design early in the course (Avalon-MM peripheral + driver + sysfs + tiny C app).
