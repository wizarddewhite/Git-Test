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

source accounts.sh
accounts_num=${#accounts[@]}

source restart_app.sh
source switch_account.sh
source next_article.sh
source tap_one_article.sh
source to_article_lists.sh
source get_dev_info.sh

randomize_accounts() {
    for src in $(eval echo "{0..$[ $accounts_num / 2 - 1 ]}")
    do
        dst=$((RANDOM % $accounts_num))
        # echo $src : ${accounts[$src]}
        # echo $dst : ${accounts[$dst]}
        tmp=${accounts[$src]}
        accounts[$src]=${accounts[$dst]}
        accounts[$dst]=$tmp
    done
}

get_dev_info
randomize_accounts

for idx in $(eval echo "{0..$[ $accounts_num - 1 ]}")
do
    echo Use account: ${accounts[$idx]}
    restart_app $width $height
    switch_account $width $height ${accounts[$idx]}
    to_article_list $width $height

    # tap 30 articles a round
    for i in {1..20}
    do
        echo Tap one article
        next_article $width $height
        tap_one_article $width $height
    done

done
