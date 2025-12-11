# LED Bus Avalon-MM Slave (`ledbus_avalon.vhd`)

## Overview

Simple Avalon-MM slave that exposes a single 32-bit register to drive the 10 user LEDs on the DE10-Nano. Intended to be mapped on the HPS lightweight bridge so user-space software can control LED patterns.

## Interface

- **Clock / reset**
  - `clk` — Avalon clock
  - `rst` — Active-high reset, clears LEDs to 0

- **Avalon-MM (slave)**
  - `avs_address` — word address (only offset `0x0` used)
  - `avs_write`, `avs_writedata` — latch new LED pattern on write
  - `avs_readdata` — reads back current LED register value

- **LED outputs**
  - `leds(9 downto 0)` — drives the 10 board LEDs

## Register map

| Offset | Address     | Name              | R/W | Description                           |
|--------|-------------|-------------------|-----|---------------------------------------|
| 0x00   | 0x0017F450  | `LED_PATTERN`     | R/W | Bits `[9:0]` drive the 10 LEDs        |

All other addresses are currently unused/reserved.

## Usage

1. Instantiate this component in Platform Designer as an Avalon-MM slave.
2. Connect it to the HPS lightweight bridge.
3. From user space, write a 32-bit value to the mapped base address (offset `0x0`) to set the LEDs.
4. Read from the same location to get the current LED pattern.

