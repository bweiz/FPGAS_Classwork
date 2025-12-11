# Software source code

## pot_to_rgb.c
This is a c program that reads the values of the potentiometers using the ADCs and the ADC driver. It then controls the color of the RGB LED using the RGB LED PWM driver. The program will also read a numbers.txt file in the home directory to set static colors.

### Compilation
Use standard c compiler for the FPGA. The following is the command to cross compile from another system
```bash
arm-linux-gnueabihf-gcc -o pot_to_rgb pot_to_rgb.c
```

### Usage
This can program can just be run on its own or through the launch script. No arguments are required when running it by itself.

## custom_pb_colors.sh
This bash script will watch for the button to be pressed through the push button driver. It will then increment the numbers.txt file in the home directory. The number will go up to 3 before resetting back to 0.

### Usage
This script can be run without any extra arguments.
```bash
bash ./custom_pb_colors.sh
```

## update_led_bar.sh
This bash script will update the led bar with the number of linux process through the led bar driver.

## Usage
This script can be run without any extra arguments
```bash
bash ./update_led_bar.sh
```

## launch.sh
This script launches everything discussed perviously and runs them concurrently.

## Usage
The script is run with two arguments. The first one specifies if `custom_pb_colors.sh` and `pot_to_rgb` should be run. The next specifies if `update_led_bar.sh` should be run.

To run everything:
```bash
bash ./launch.sh y y
```

To run just `update_led_bar.sh`:
```bash
bash ./launch.sh n y
```