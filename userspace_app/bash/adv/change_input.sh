#!/bin/bash

if [[ "adb" == "${1}" ]]; then
    echo Change to ADB
    adb shell ime set com.android.adbkeyboard/.AdbIME
else
    echo Change to Fly
    adb shell ime set com.iflytek.inputmethod/.FlyIME
fi
