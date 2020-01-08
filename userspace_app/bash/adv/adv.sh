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

accounts=( \
	"18017547416" "18916874492" \
	)

source restart_app.sh
source switch_account.sh
source next_article.sh
source tap_one_article.sh
source to_article_lists.sh
source get_dev_info.sh

get_dev_info

for (( ; ; ))
do
    second=`date +%s`
    idx=$((second % 2))
    echo Use account: ${accounts[$idx]}
    restart_app $width $height
    switch_account $width $height ${accounts[$idx]}
    to_article_list $width $height

    # tap 10 articles a round
    for i in {1..10}
    do
        echo Tap one article
        tap_one_article $width $height
        next_article $width $height
    done

done
