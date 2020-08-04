#!/bin/bash

if [ $# != 2 ]; then
	echo "Usage: ./analysis_result result_dir1 result_dir2"
	exit
fi

if [ ! -d "$1" ]; then
	echo "No such \"$1\" directory"
	exit
fi

if [ ! -d "$2" ]; then
	echo "No such \"$2\" directory"
	exit
fi

printf "Result in %-15s:" $1 
./redis_average.sh -r $1 | awk '{print "   " $2}' | tee .tmp_r1
printf "Result in %-15s:" $2
./redis_average.sh -r $2 | awk '{print "   " $2}' | tee .tmp_r2

awk -f get_diff.awk .tmp_r1 .tmp_r2




