#!/bin/bash

if [ $# != 1 ]; then
	echo "Usage: ./oom.sh dir"
	exit
fi

cd $1
gcc oom.c -o oom

# run redis benchmark
for ((i = 0; i < 3; i++));
do
	if ! (($i % 5)); then
		sleep 1
	fi
	./oom &
	pids[$i]=$!
done

# wait for children to finish
for pid in ${pids[*]}; do
	wait $pid
done

rm -f oom
echo Done
