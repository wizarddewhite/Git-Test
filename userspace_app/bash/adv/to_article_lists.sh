#!/bin/bash

to_article_list() {
    # open contact list
    adb shell input tap 300 1900
    sleep 1
    # open public
    adb shell input tap 500 700
    sleep 1
    # open my account
    adb shell input tap 500 200
    sleep 1
    # open list
    adb shell input tap 1000 93
    # adjust position
    adb shell input swipe 500 620 500 300
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    to_article_list
fi
