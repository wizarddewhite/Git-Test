#!/bin/bash

total_mem=`cat /proc/meminfo | grep MemTotal | awk '{print $2}'`
echo total_mem $total_mem
# 10G = 10485760k
let "reserve_mem = $total_mem - 10485760"
echo reserve_mem $reserve_mem
# 2M = 2048k
let "reserve_pages = $reserve_mem / 2048"
echo reserve_pages $reserve_pages
