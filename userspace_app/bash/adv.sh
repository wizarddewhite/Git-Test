#!/bin/bash

# adb start-server

# https://blog.bihe0832.com/adb-shell-input.html

for (( ; ; ))
do
#enter article
adb shell input tap 512 1520

# scroll to top
adb shell input swipe 500 300 500 1800
adb shell input swipe 500 300 500 1800

# scroll down to point
adb shell input swipe 500 1600 500 200

sleep 2

# enter adv
adb shell input tap 512 1520
sleep 5

# back adv
adb shell input keyevent 4
# back article
adb shell input keyevent 4
sleep 5

#take a look into another article
adb shell input tap 512 800
sleep 1
adb shell input swipe 500 300 500 1800
# back article
adb shell input keyevent 4

done
