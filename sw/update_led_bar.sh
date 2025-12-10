SW_LED_CONTROL="/sys/devices/platform/ff37f450.ledbar/sw_led_control"

while true
do
    proc_count=$(ps -e --no-headers | wc -l)
    echo proc_count > $SW_LED_CONTROL
done