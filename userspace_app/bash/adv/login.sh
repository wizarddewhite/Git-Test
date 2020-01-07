#!/bin/bash

log_in() {
    # switch account
    adb shell input tap 750 1850
    sleep 2
    adb shell input tap 500 1400

    # log in
    adb shell am broadcast -a ADB_INPUT_TEXT --es msg ${1}
    adb shell input tap 500 1100
    sleep 8
    adb shell am broadcast -a ADB_INPUT_TEXT --es msg '144025shyw'
    adb shell input tap 500 1100
    sleep 8
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    log_in $@
fi
