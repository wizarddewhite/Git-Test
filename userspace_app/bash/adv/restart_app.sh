#!/bin/bash

restart_app() {
    adb shell am force-stop com.tencent.mm
    sleep 0.5
    # need to put icon to this point
    x=`echo "($width*0.185)/1" | bc`
    y=`echo "($height*0.156)/1" | bc`
    adb shell input tap $x $y
    sleep 10 # wait for launch
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    source get_dev_info.sh
    get_dev_info
    echo Width:  $width
    echo Height: $height
    restart_app $width $height
fi
