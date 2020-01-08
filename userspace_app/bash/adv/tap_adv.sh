#!/bin/bash

tap_adv() {
    # enter adv
    echo enter adv
    x=`echo "($width*0.925)/1" | bc`
    y=`echo "($height*0.625)/1" | bc`
    adb shell input tap $x $y
    sleep 18
    
    # back adv
    adb shell input keyevent 4
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    source get_dev_info.sh
    get_dev_info
    echo Width:  $width
    echo Height: $height
    tap_adv $width $height
fi
