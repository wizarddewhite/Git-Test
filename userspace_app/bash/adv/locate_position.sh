#!/bin/bash

locate_position() {
    width=$1
    height=$2
    # open menu
    sleep 1
    x=`echo "($width*0.925)/1" | bc`
    y=`echo "($height*0.048)/1" | bc`
    adb shell input tap $x $y
    sleep 1
    # open search
    y=`echo "($height*0.833)/1" | bc`
    adb shell input tap $x $y
    sleep 1
    adb shell am broadcast -a ADB_INPUT_TEXT --es msg '在看'
    sleep 5
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    source get_dev_info.sh
    get_dev_info
    echo Width:  $width
    echo Height: $height
    locate_position $width $height
fi
