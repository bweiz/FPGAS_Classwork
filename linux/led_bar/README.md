# Driver for the DE10 Nano

External, 10 LED LEDBAR

## Building

The Makefile in this directory cross-compiles the driver. Update the `KDIR` variable to point to your linux-socfpga repository directory.

Run `make` in this directory to build to kernel module.

## Device tree node

ledbar: ledbar@ff37f450 { 
        compatible = "sdc,led_bar"; 
        reg = <0xff37f450 32>; 
};
```

## Notes / bugs :bug:

Span in dts is 32 bytes, in reality should be 16 bytes. Hasn't posed an issue yet.

## Register map

Only write to lower 10 bits of.

| Offset | Name         | R/W | Purpose                     |
|--------|--------------|-----|-----------------------------|
| 0x0    | led_pattern  | W   | Register to control 10 LEDs |



