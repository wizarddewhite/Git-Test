#!/bin/bash

locate_position() {
    # open menu
    sleep 1
    adb shell input tap 1000 93
    sleep 1
    # open search
    adb shell input tap 1000 1600
    sleep 1
    adb shell am broadcast -a ADB_INPUT_TEXT --es msg '在看'
    sleep 1
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    locate_position
fi
