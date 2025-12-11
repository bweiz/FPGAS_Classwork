BUTTON_PRESS="/sys/devices/platform/ff37f470.pushbutton/push_button_reg"
number=0
while true
do
    button_state=$(cat $BUTTON_PRESS)
    while [ "$button_state" -eq 0 ]
    do
    button_state=$(cat $BUTTON_PRESS)
    done
    number=$(( (number + 1) % 4 ))
    echo $number > /home/soc/number.txt
    echo 0 > $BUTTON_PRESS
done