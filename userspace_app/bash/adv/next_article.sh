#!/bin/bash

next_article() {
    times=$((1 + RANDOM % 8))
    width=$1
    height=$2
    # x at the middle
    x=$((width / 2))
    # y1 -> y2 from bottom to up with delta of one article
    y1=$((height-200))
    # we assume one article has a height of 0.29 screen
    delta=`echo "($height*0.245)/1" | bc`
    y2=$((y1 - delta))
    for i in $(eval echo "{1..$times}")
    do
        # move up one
        adb shell input swipe $x $y1 $x $y2
    done
    sleep 1
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    source get_dev_info.sh
    get_dev_info
    echo Width:  $width
    echo Height: $height
    next_article $width $height
fi
