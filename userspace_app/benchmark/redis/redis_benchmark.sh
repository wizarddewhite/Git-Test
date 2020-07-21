#!/bin/bash

# default loops
times=10

if [ $# == 1 ]; then
    times=$1
fi

echo Total run $times instance

mkdir -p result

for ((i = 1; i <= $times; i++));
do
    ( ./redis_solo.sh $i $PWD/result & )
done
