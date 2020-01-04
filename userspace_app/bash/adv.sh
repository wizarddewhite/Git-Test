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
    sleep 1
    adb shell input tap 1000 93
    sleep 1
    # open search
    adb shell input tap 1000 1600
    sleep 1
    adb shell am broadcast -a ADB_INPUT_TEXT --es msg '在看'
    sleep 1
}

tap_one_artical() {
    #enter article
    adb shell input tap $1 $2
    
    locate_position
    
    # enter adv
    echo enter adv
    adb shell input tap 512 1300
    sleep 18
    
    # back adv
    adb shell input keyevent 4
    # back article
    adb shell input tap 95 93
    sleep 1
}

to_subscribe_list() {
    # go out and enter again
    adb shell input keyevent 4

    # to top level
    adb shell input tap 95 93
    adb shell input tap 95 93
    adb shell input tap 95 93
    adb shell input tap 95 93

    # subscribe
    adb shell input tap 95 210
    # into list
    adb shell input tap 1000 93
    # my channel
    adb shell input tap 95 210
    # show channel content
    adb shell input tap 1000 93
}

for (( ; ; ))
do
    adb shell input swipe 500 620 500 300
    # tap 100 articles a round
    for i in {1..100}
    do
        echo Tap one article
        tap_one_artical 512 1520

        # move up one
        adb shell input swipe 500 860 500 300
        sleep 1
    done

    to_subscribe_list
done
