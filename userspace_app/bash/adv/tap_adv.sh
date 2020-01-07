#!/bin/bash

tap_adv() {
    # enter adv
    echo enter adv
    adb shell input tap 1000 1200
    sleep 18
    
    # back adv
    adb shell input keyevent 4
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    tap_adv $@
fi
