#!/bin/sh
cd /sys/bus/platform/devices/ff37f430.rgb_pwm

# set period once
echo 320 > period

# red
echo 65535 > red
echo 0 > green
echo 0 > blue
sleep 1

# green
echo 0 > red
echo 65535 > green
echo 0 > blue
sleep 1

# blue
echo 0 > red
echo 0 > green
echo 65535 > blue
sleep 1

# white
echo 65535 > red
echo 65535 > green
echo 65535 > blue