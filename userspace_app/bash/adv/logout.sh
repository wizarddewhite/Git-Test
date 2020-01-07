#!/bin/bash

log_out() {
    # setting
    adb shell input tap 900 1800
    sleep 2
    adb shell input tap 500 1400
    sleep 2
    # quit
    adb shell input tap 500 1900
    adb shell input tap 500 1450
    adb shell input tap 750 1200
    sleep 20 # wait for quit
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    log_out
fi
