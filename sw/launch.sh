
pb_pid=""
pot_pid=""
bar_pid=""

if [ $1 = "y" ]; then
    pb_pid=$(bash ./custom_pb_colors.sh &)
    pot_pid=$(./pot_to_rgb &)
fi

if [ $2 = "y" ]; then
    bar_pid=$(bash ./update_led_bar.sh &)
fi


read -p "Press enter to exit" response


if [ $1 = "y" ]; then
    kill "$pb_pid"
    kill "$pot_pid"
fi

if [ $2 = "y" ]; then
    kill "$bar_pid"
fi
