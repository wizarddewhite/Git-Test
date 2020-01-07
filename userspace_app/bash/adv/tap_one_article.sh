#!/bin/bash
source locate_position.sh
source tap_adv.sh

tap_one_article() {
    #enter article
    adb shell input tap $1 $2
    sleep 1

    locate_position

    tap_adv

    # back article
    adb shell input tap 95 93
    sleep 1
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    tap_one_article 512 1520
fi
