#!/bin/bash
source login.sh
source logout.sh

switch_account() {
    log_out $width $height

    log_in $width $height ${3}
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    source get_dev_info.sh
    get_dev_info
    echo Width:  $width
    echo Height: $height
    switch_account $width $height $@
fi
