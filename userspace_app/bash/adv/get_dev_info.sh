#!/bin/bash

get_dev_info() {
    size=`adb shell wm size | awk '{print $3}' | sed -e 's/[[:space:]]*$//'`
    width=`echo $size | awk -F"x" '{print $1}'`
    height=`echo $size | awk -F"x" '{print $2}'`
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    get_dev_info
    echo Width:  $width
    echo Height: $height
fi
