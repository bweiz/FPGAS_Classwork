# LED Patterns Avalon Slave (`led_patterns_avalon.vhd`)

## Overview

Avalon-MM slave around a **debounced push button**.  
Exposes a 1-bit `button_status` register that can be read by the HPS and optionally written/cleared from software.

## Interface

- **Clock / reset**
  - `clk` — system / Avalon clock  
  - `rst` — active-high reset, clears internal state

- **Avalon-MM (slave)**
  - `avs_read` / `avs_readdata` — read current `button_status`
  - `avs_write` / `avs_writedata` — write `button_status`
  - `avs_address(1 downto 0)` — only `"00"` used

- **External I/O**
  - `push_button` — raw push button input (debounced internally)

## Register map

| Offset | Name           | Bits    | R/W | Description                                  |
|--------|----------------|---------|-----|----------------------------------------------|
| 0x0    | button_status  | [0]     | R/W | Latched button status                        |
|        |                | [31:1]  | R   | Reads back as `0`                            |

## Behavior

- `push_button` is debounced using a 25 MHz-ish timer.
- A valid debounced press sets `button_status` to `1`.
- Software can:
  - **Read** offset `0x0` to check if a press occurred.
  - **Write** offset `0x0` (bit 0) to clear or force `button_status`.

## Usage

1. Instantiate as an Avalon-MM slave and connect to the HPS lightweight bridge.
2. From user space, `mmap` the base address and:
   - Read `button_status` to detect presses.
   - Write `0` to bit 0 after handling to clear the latched state.

