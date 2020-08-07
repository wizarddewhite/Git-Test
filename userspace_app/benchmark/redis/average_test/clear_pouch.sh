#!/bin/bash

names=`pouch ps -a | awk  'NR!=1{print $1}'`
eval "arr=($names)"
for c in "${arr[@]}"; do
	pouch rm -f  $c
done

