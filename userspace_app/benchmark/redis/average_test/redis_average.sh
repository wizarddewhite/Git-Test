#!/bin/bash

# default loops
times=10
result_dir="result"
runtime="runc"

while getopts "dhr:t:" opt; do
	case "$opt" in
	"r")
		result_dir=$OPTARG
		;;
	"h")
		echo "Usage: redis_average.sh -r result_dir"
		exit
		;;
	esac
done

if [ ! -d "$result_dir" ]; then
	echo "No such \"$result_dir\" directory"
	exit
fi

# cleanup or create
rm -f $result_dir/result_summary
touch $result_dir/result_summary
rm -f $result_dir/.result
rm -f $result_dir/result_average

for file in `ls $result_dir`
do
	if [[ $file == "result_summary" ]]; then
		continue
	fi
	sed -e 's//\n/g' $result_dir/$file > $result_dir/.result
	awk '/per second/ {print $2}' $result_dir/.result >> $result_dir/result_summary
done

awk -f get_average.awk $result_dir/result_summary > $result_dir/result_average
cat $result_dir/result_average
