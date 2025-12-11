# Driver for Push Button

4 prong, external push button

## Building

The Makefile in this directory cross-compiles the driver. Update the `KDIR` variable to point to your linux-socfpga repository directory.

Run `make` in this directory to build to kernel module.

## Device tree node

    pushbutton: pushbutton@ff37f470 { 
        compatible = "sdc,push_button"; 
        reg = <0xff37f470 32>; 
    }; 
```

## Notes / bugs :bug:

Span in dts is 32 bytes, in reality should be 16 bytes. Hasn't posed an issue yet.

## Register map

Only use first bit of register

| Offset | Name         | R/W | Purpose                     |
|--------|--------------|-----|-----------------------------|
| 0x0    | button_press | R   | Register shows button state |



