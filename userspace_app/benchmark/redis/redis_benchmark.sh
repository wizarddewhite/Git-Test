#!/bin/bash

# default loops
times=10
result_dir="result"

while getopts "hr:t:" opt; do
	case "$opt" in
	"t")
		times=$OPTARG
		;;
	"r")
		result_dir=$OPTARG
		;;
	"h")
		echo "Usage: redis_benchmark.sh -t times -r result_dir"
		exit
		;;
	esac
done

echo Total run $times instance

mkdir -p $result_dir

for ((i = 1; i <= $times; i++));
do
    ( ./redis_solo.sh $i $PWD/$result_dir & )
done
