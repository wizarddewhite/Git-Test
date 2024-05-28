#!/bin/bash

usage()
{
	echo "Usage: print mem size from /proc/meminfo"
	echo $0 [-h]
	exit
}

while getopts "h" opt; do
	case "$opt" in
	"h")
		usage
		;;
	":")
		echo "no argument for option: $OPTARG"
		;;
	"?")
		# this works when ":" is the first letter in optstring
		echo "not valid option: $OPTARG"
		;;
	esac
done


total_mem=`cat /proc/meminfo | grep MemTotal | awk '{print $2}'`
echo total_mem $total_mem
# 10G = 10485760k
let "reserve_mem = $total_mem - 10485760"
echo reserve_mem $reserve_mem
# 2M = 2048k
let "reserve_pages = $reserve_mem / 2048"
echo reserve_pages $reserve_pages
