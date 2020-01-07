#!/bin/bash
source login.sh
source logout.sh

switch_account() {
    log_out

    log_in ${1}
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    switch_account $@
fi
