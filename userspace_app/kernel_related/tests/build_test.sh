#!/bin/bash

SCRIPT_FILE=$0
SCRIPT_DIR=$(dirname $SCRIPT_FILE)

source $SCRIPT_DIR/memblock_test.sh

while [ true ]; do make clean && time make -j8 bzImage &> /dev/null && date; done
