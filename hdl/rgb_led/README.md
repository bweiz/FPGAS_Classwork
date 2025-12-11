# RGB PWM Controller Avalon Subsystem  
Files: `pwm_rgb_avalon.vhd`, `pwm_rgb.vhd`, `pwm_controller.vhd`

## Overview

Hardware RGB LED dimmer controlled over Avalon-MM.  
Software writes fixed-point duty cycles and a period into registers; hardware generates three PWM signals (`pwm_r/g/b`) for an external RGB LED.

- `pwm_rgb_avalon.vhd` – Avalon-MM slave + register file
- `pwm_rgb.vhd` – connects registers to three PWM channels
- `pwm_controller.vhd` – core fixed-point PWM controller

## Memory Map (Avalon-MM)

Base: `0x0017F430`  
Span: `0x10` bytes (`0x0017F430`–`0x0017F43F`)

| Offset | Address     | Name       | Width      | R/W | Description                                  |
|--------|-------------|------------|------------|-----|----------------------------------------------|
| 0x0    | 0x0017F430  | DUTY_R     | 18.17 fp   | R/W | Red duty cycle (0.0–1.0)                     |
| 0x4    | 0x0017F434  | DUTY_G     | 18.17 fp   | R/W | Green duty cycle                             |
| 0x8    | 0x0017F438  | DUTY_B     | 18.17 fp   | R/W | Blue duty cycle                              |
| 0xC    | 0x0017F43C  | PERIOD     | 11.5 fp ms | R/W | PWM period in ms (shared by all channels)    |

Reads return zero-padded register contents.

## Fixed-Point Formats

- `duty_*` (18.17 fixed-point):  
  - `0` = 0% duty, `DUTY_SCALE` (`1 << 17`) = 100% duty.
- `period` (11.5 fixed-point ms):  
  - Converted internally to clock cycles using `CLK_PERIOD` (default 20 ns).

## Usage

1. **Instantiate** `pwm_rgb_avalon` as Avalon-MM slave, connect to HPS lightweight bridge.
2. **Hook up pins**: `pwm_r/g/b` to the RGB LED (or header).
3. From Linux/user space:
   - Write `DUTY_R/G/B` to set color brightness.
   - Write `PERIOD` to adjust PWM frequency.
   - Optionally read back registers for debugging.

This block is used by user-space tools to set RGB color/intensity via memory-mapped I/O.

