#!/bin/bash

echo "from 0 to 10"
for a in 0 1 2 3 4 5 6 7 8 9 10;
do 
	echo "  " $a;
done

echo "echo file name"
for file in `ls`;
do
	echo "  " $file;
done

echo "from 10 to 19"
for ((i=10;i<20;i++));
do
	echo "  " $i;
done

# iterate command line output
local_files=`ls 2>/dev/null`
IFS=$'\n'
local_files=($local_files)
for (( i=0; i<${#local_files[@]}; i++ ))
do
    echo ${local_files[$i]}
done

