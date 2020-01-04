#!/bin/bash

# adb start-server

# input chinese from https://github.com/senzhk/ADBKeyBoard
# install and setup
#   adb install -r ADBKeyBoard.apk
# set to use ADBKeyBoard
#   adb shell ime enable com.android.adbkeyboard/.AdbIME
#   adb shell ime set com.android.adbkeyboard/.AdbIME
# set to use FlyIME
#   adb shell ime enable com.iflytek.inputmethod/.FlyIME
#   adb shell ime set com.iflytek.inputmethod/.FlyIME

locate_position() {
# open menu
adb shell input tap 1000 93
sleep 3
# open search
adb shell input tap 1000 1600
sleep 0.5
adb shell am broadcast -a ADB_INPUT_TEXT --es msg '在看'
sleep 2
}

tap_one_artical() {
#enter article
adb shell input tap 512 1520

locate_position

# enter adv
echo enter adv
adb shell input tap 512 1300
sleep 20

# back adv
adb shell input keyevent 4
# back article
adb shell input tap 95 93
sleep 5
}

for (( ; ; ))
do

echo Tap one article
tap_one_artical

echo Take a look on another article
#take a look into another article
adb shell input tap 512 800
sleep 1
adb shell input swipe 500 300 500 1800
# back article
adb shell input keyevent 4

done
