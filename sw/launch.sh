
pb_pid=""
pot_pid=""
bar_pid=""

if [ $1 = "y" ]; then
    bash ./custom_pb_colors.sh &
    pb_pid=$!
    ./pot_to_rgb &
    pot_pid=$!
fi

if [ $2 = "y" ]; then
    bash ./update_led_bar.sh &
    bar_pid=$!
fi


read -p "Press enter to exit" response


if [ $1 = "y" ]; then
    kill "$pb_pid"
    kill "$pot_pid"
fi

if [ $2 = "y" ]; then
    kill "$bar_pid"
fi
