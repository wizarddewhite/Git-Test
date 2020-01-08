#!/bin/bash

log_out() {
    width=$1
    height=$2
    # setting
    x=`echo "($width*0.833)/1" | bc`
    y=`echo "($height*0.937)/1" | bc`
    adb shell input tap $x $y
    sleep 2
    x=$((width / 2))
    y=`echo "($height*0.729)/1" | bc`
    adb shell input tap $x $y
    sleep 2
    # quit
    y=`echo "($height*0.989)/1" | bc`
    adb shell input tap $x $y
    y=`echo "($height*0.755)/1" | bc`
    adb shell input tap $x $y
    x=`echo "($width*0.694)/1" | bc`
    y=`echo "($height*0.625)/1" | bc`
    adb shell input tap $x $y
    sleep 20 # wait for quit
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    source get_dev_info.sh
    get_dev_info
    echo Width:  $width
    echo Height: $height
    log_out $width $height
fi
