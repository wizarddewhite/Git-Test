#!/bin/bash
source locate_position.sh
source tap_adv.sh

tap_one_article() {
    width=$1
    height=$2
    #enter article
    x=$((width / 2))
    y=`echo "($height*0.791)/1" | bc`
    adb shell input tap $x $y
    sleep 1

    locate_position $width $height

    tap_adv $width $height

    # back article
    x=`echo "($width*0.087)/1" | bc`
    y=`echo "($height*0.048)/1" | bc`
    adb shell input tap $x $y
    sleep 1
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    source get_dev_info.sh
    get_dev_info
    echo Width:  $width
    echo Height: $height
    tap_one_article $width $height
fi
