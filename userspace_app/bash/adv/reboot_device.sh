#!/bin/bash

reboot_device() {
    width=$1
    height=$2
    adb reboot
    # wait for bootup
    sleep 200
    # light up screen
    # adb shell input keyevent 26
    # unlock the screen
    # x=$((width / 2))
    # y1=$((height-200))
    # delta=`echo "($height*0.5)/1" | bc`
    # y2=$((y1 - delta))
    # adb shell input swipe $x $y1 $x $y2 50
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    source get_dev_info.sh
    get_dev_info
    echo Width:  $width
    echo Height: $height
    reboot_device $width $height
fi
