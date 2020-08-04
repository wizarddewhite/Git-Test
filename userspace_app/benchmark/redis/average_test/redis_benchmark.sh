#!/bin/bash

# default loops
inject="no"
result_dir="result"
runtime="runc"
times=1

while getopts "dhr:t:i" opt; do
	case "$opt" in
	"t")
		times=$OPTARG
		;;
	"i")
		inject="yes"
		;;
	"r")
		result_dir=$OPTARG
		;;
	"d")
		runtime="rund"
		;;
	"h")
		echo "Usage: redis_benchmark.sh -d -i -t times -r result_dir"
		exit
		;;
	esac
done

echo Total run $times instance

mkdir -p $result_dir

# run redis benchmark
for ((i = 0; i < $times; i++));
do
	if ! (($i % 5)); then
		sleep 1
	fi
    ( ./redis_solo.sh $i $PWD/$result_dir $runtime & )
done

# inject oom?

if [[ $inject == "no" ]]; then
	exit 0
fi

sleep 30

nums=`pouch ps -a  | wc -l`
while [[ $nums -lt $times ]];
do
	sleep 1
	echo $nums pos up
	nums=`pouch ps -a  | wc -l`
done


echo inject oom!!!
./inject_oom.sh $PWD $runtime
