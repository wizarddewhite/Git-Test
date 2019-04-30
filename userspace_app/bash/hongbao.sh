#!/bin/bash

for (( ; ; ))
do
#enter first entry
adb shell input tap 512 220
#sleep 0.1

# tap hongbao
adb shell input tap 612 1700
adb shell input tap 612 1500
#adb shell input tap 612 1200
#sleep 0.3

# catch hongbao
adb shell input tap 512 1300
#sleep 0.2

# back 
adb shell input keyevent 4
#adb shell input tap 95 93
adb shell input tap 95 93
adb shell input tap 95 93
done
