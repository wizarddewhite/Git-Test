#!/bin/bash

next_article() {
    times=$((1 + RANDOM % 3))
    for i in $(eval echo "{1..$times}")
    do
        # move up one
        adb shell input swipe 500 860 500 302
    done
    sleep 1
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    next_article
fi
