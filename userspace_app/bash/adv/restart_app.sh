#!/bin/bash

restart_app() {
    adb shell am force-stop com.tencent.mm
    sleep 0.5
    # need to put icon to this point
    adb shell input tap 200 300
    sleep 10 # wait for launch
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    restart_app
fi
