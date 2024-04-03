#!/bin/bash

while [ true ]; do make clean && time make -j8 bzImage &> /dev/null; done
