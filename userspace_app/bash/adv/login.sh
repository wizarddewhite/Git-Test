#!/bin/bash

log_in() {
    width=$1
    height=$2
    # switch account
    x=`echo "($width*0.694)/1" | bc`
    y=`echo "($height*0.963)/1" | bc`
    adb shell input tap $x $y
    sleep 2
    x=$((width / 2))
    y=`echo "($height*0.729)/1" | bc`
    adb shell input tap $x $y
    sleep 2

    # log in
    input=`echo "($height*0.364)/1" | bc`
    y=`echo "($height*0.572)/1" | bc`
    adb shell input tap $x $input
    adb shell am broadcast -a ADB_INPUT_TEXT --es msg ${3}
    adb shell input tap $x $y
    sleep 8
    adb shell input tap $x $input
    adb shell am broadcast -a ADB_INPUT_TEXT --es msg '144025shyw'
    adb shell input tap $x $y
    sleep 8
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    source get_dev_info.sh
    get_dev_info
    echo Width:  $width
    echo Height: $height
    log_in $width $height $@
fi
