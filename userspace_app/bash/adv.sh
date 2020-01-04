#!/bin/bash

# adb start-server

locate_position() {
# scroll to top
adb shell input swipe 500 300 500 1800
adb shell input swipe 500 300 500 1800

# scroll down to point
adb shell input swipe 500 1600 500 200

sleep 2
}

for (( ; ; ))
do
#enter article
adb shell input tap 512 1520

locate_position

# enter adv
adb shell input tap 512 1520
sleep 5

# back adv
adb shell input keyevent 4
# back article
adb shell input tap 95 93
sleep 5

#take a look into another article
adb shell input tap 512 800
sleep 1
adb shell input swipe 500 300 500 1800
# back article
adb shell input keyevent 4

done
