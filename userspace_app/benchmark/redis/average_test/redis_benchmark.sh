#!/bin/bash

# default loops
times=10
result_dir="result"
runtime="runc"

while getopts "dhr:t:" opt; do
	case "$opt" in
	"t")
		times=$OPTARG
		;;
	"r")
		result_dir=$OPTARG
		;;
	"d")
		runtime="rund"
		;;
	"h")
		echo "Usage: redis_benchmark.sh -d -t times -r result_dir"
		exit
		;;
	esac
done

echo Total run $times instance

mkdir -p $result_dir

for ((i = 1; i <= $times; i++));
do
	if ! (($i % 4)); then
		sleep 1
	fi
    ( ./redis_solo.sh $i $PWD/$result_dir $runtime & )
done
