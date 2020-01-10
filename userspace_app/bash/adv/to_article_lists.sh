#!/bin/bash

to_article_list() {
    width=$1
    height=$2
    # open contact list
    x=`echo "($width*0.28)/1" | bc`
    y=$((height-50))
    adb shell input tap $x $y
    sleep 2
    # open public
    y=`echo "($height*0.355)/1" | bc`
    adb shell input tap $x $y
    sleep 2
    # open my account
    y=`echo "($height*0.104)/1" | bc`
    adb shell input tap $x $y
    sleep 2
    # open list
    x=`echo "($width*0.925)/1" | bc`
    y=`echo "($height*0.048)/1" | bc`
    adb shell input tap $x $y
    sleep 2
    # adjust position
    x=$((width / 2))
    # y1 -> y2 from bottom to up with adjustment
    y1=$((height-200))
    delta=`echo "($height*0.16)/1" | bc`
    y2=$((y1 - delta))
    # adb shell input swipe $x $y1 $x $y2
    sleep 2
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    source get_dev_info.sh
    get_dev_info
    echo Width:  $width
    echo Height: $height
    to_article_list $width $height
fi
