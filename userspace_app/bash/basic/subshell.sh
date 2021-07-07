#!/bin/bash

function f1() 
{
	sleep 1
	echo "this is funciton 1"
}

function f2() 
{
	sleep 1
	echo "this is funciton 2"
}

# run in a subshell, but not wait
( f1 & )

# run in job and wait
for i in `seq 0 3`; do
  f2 &
done

echo exits
wait
