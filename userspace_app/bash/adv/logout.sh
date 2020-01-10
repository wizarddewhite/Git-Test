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
    y=`echo "($height*0.6)/1" | bc`
    adb shell input tap $x $y
    sleep 2
    # quit
    y=`echo "($height*0.857)/1" | bc`
    adb shell input tap $x $y
    y=`echo "($height*0.791)/1" | bc`
    adb shell input tap $x $y
    x=`echo "($width*0.555)/1" | bc`
    y=`echo "($height*0.592)/1" | bc`
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
