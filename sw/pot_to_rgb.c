// pot_to_rgb.c
// Read ADC channels 0–2 and drive RGB PWM via sysfs.
// Assum:
//   ADC  at /sys/bus/platform/devices/ff37f400.adc
//   RGB  at /sys/bus/platform/devices/ff37f430.rgb_pwm

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define ADC_SYSFS_BASE   "/sys/bus/platform/devices/ff37f400.adc"
#define RGB_SYSFS_BASE   "/sys/bus/platform/devices/ff37f430.rgb_pwm"
#define BUTTON_PATH      "/home/soc/number.txt"

// ADC attributes
#define ADC_CH0_RAW      ADC_SYSFS_BASE "/ch0_raw"
#define ADC_CH1_RAW      ADC_SYSFS_BASE "/ch1_raw"
#define ADC_CH2_RAW      ADC_SYSFS_BASE "/ch2_raw"
#define ADC_AUTO_UPDATE  ADC_SYSFS_BASE "/auto_update"
#define ADC_UPDATE       ADC_SYSFS_BASE "/update"

// RGB PWM attributes
#define RGB_PERIOD       RGB_SYSFS_BASE "/period"
#define RGB_RED          RGB_SYSFS_BASE "/red"
#define RGB_GREEN        RGB_SYSFS_BASE "/green"
#define RGB_BLUE         RGB_SYSFS_BASE "/blue"

// duty is 18.17 => scale by 2^17
#define DUTY_SCALE       (1u << 17)

// write an unsigned integer to a sysfs file
static int write_u32(const char *path, uint32_t value)
{
    FILE *f = fopen(path, "w");
    if (!f) {
        fprintf(stderr, "Failed to open %s for write: %s\n",
                path, strerror(errno));
        return -1;
    }

    if (fprintf(f, "%u\n", value) < 0) {
        fprintf(stderr, "Failed to write to %s: %s\n",
                path, strerror(errno));
        fclose(f);
        return -1;
    }

    fclose(f);
    return 0;
}

// write "1" or "0" (bool) to sysfs
// return 0 if successful
static int write_bool(const char *path, int value)
{
    FILE *f = fopen(path, "w");
    if (!f) {
        fprintf(stderr, "Failed to open %s for write: %s\n",
                path, strerror(errno));
        return -1;
    }

    if (fprintf(f, "%d\n", value ? 1 : 0) < 0) {        // if value nonzero, write 1, else 0
        fprintf(stderr, "Failed to write to %s: %s\n",
                path, strerror(errno));
        fclose(f);
        return -1;
    }

    fclose(f);
    return 0;
}

// read a 16-bit ADC value from sysfs (ch*_raw)
// return 0 if successful
static int read_u16(const char *path, uint16_t *out)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Failed to open %s for read: %s\n",
                path, strerror(errno));
        return -1;
    }

    unsigned int tmp = 0;
    if (fscanf(f, "%u", &tmp) != 1) {
        fprintf(stderr, "Failed to parse integer from %s\n", path);
        fclose(f);
        return -1;
    }

    fclose(f);

    if (tmp > 0xFFFF)
        tmp = 0xFFFF;

    *out = (uint16_t)tmp;
    return 0;
}

// Map 12-bit ADC (0 to 4095) to 18.17 fixed-point duty (0 to 1.0)
static uint32_t adc_to_duty(uint16_t adc)
{
    // Avoid divide-by-zero; ADC is 12-bit in hardware, so 4095 is max.
    const uint32_t ADC_MAX = 4095u;

    // Scale 0 to ADC_MAX to 0 to DUTY_SCALE (roughly 0 to 1.0 in 18.17)
    uint32_t duty = (uint32_t)adc * DUTY_SCALE / ADC_MAX;

    return duty;
}

int main(void)
{
    uint16_t adc_r = 0, adc_g = 0, adc_b = 0, button_num = 0;

    printf("pot_to_rgb: starting\n");

    // Enable auto-update in the ADC
    if (write_bool(ADC_AUTO_UPDATE, 1) != 0) {
        fprintf(stderr, "Failed to enable auto_update on ADC\n");
        return 1;
    }

    
    if (write_u32(RGB_PERIOD, 320) != 0) {
        fprintf(stderr, "Failed to set RGB period\n");
        return 1;
    }

    usleep(100000);

    // Busy loop: read pots and update RGB channels
    while (1) {
        if (read_u16(ADC_CH0_RAW, &adc_r) != 0 ||
            read_u16(ADC_CH1_RAW, &adc_g) != 0 ||
            read_u16(ADC_CH2_RAW, &adc_b) != 0) {

            fprintf(stderr, "Error reading ADC channels\n");
            
            usleep(100000);
            continue;
        }

        if (read_u16(BUTTON_PATH, &button_num) != 0) {
            button_num = 0;
        }

        uint32_t duty_r = adc_to_duty(adc_r);
        uint32_t duty_g = adc_to_duty(adc_g);
        uint32_t duty_b = adc_to_duty(adc_b);

        switch (button_num) {
            case 1:
                duty_r = 0xff * DUTY_SCALE / 0xff;
                duty_g = 0x0;
                duty_b = 0x0;
                break;
            case 2:
                duty_r = 0x0;
                duty_g = 0xff * DUTY_SCALE / 0xff;
                duty_b = 0x0;
                break;
            case 3:
                duty_r = 0x0;
                duty_g = 0x0;
                duty_b = 0xff * DUTY_SCALE / 0xff;
                break;
        }

        // Write to RGB PWM sysfs
        if (write_u32(RGB_RED, duty_r) != 0 ||
            write_u32(RGB_GREEN, duty_g) != 0 ||
            write_u32(RGB_BLUE, duty_b) != 0) {

            fprintf(stderr, "Error writing RGB duties\n");
            usleep(100000);
            continue;
        }
        // Small delay to avoid busy-looping too fast
        usleep(20000);
    }

    return 0;
}
