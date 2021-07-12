#!/bin/bash

function iterate_number()
{
	echo "from 0 to 10"
	for a in 0 1 2 3 4 5 6 7 8 9 10;
	do 
		echo "  " $a;
	done

	echo "from 10 to 19"
	for ((i=10;i<20;i++));
	do
		echo "  " $i;
	done
}

function list_files()
{
	echo "echo file name"
	for file in `ls`;
	do
		echo "  " $file;
	done
}

function iterate_output()
{
	# iterate command line output
	local_files=`ls 2>/dev/null`
	IFS=$'\n'
	local_files=($local_files)
	for (( i=0; i<${#local_files[@]}; i++ ))
	do
	    echo ${local_files[$i]}
	done
}

function iterate_array()
{
	## declare an array variable
	declare -a arr=("element1" "element2" "element3")

	## now loop through the above array
	for i in "${arr[@]}"
	do
	   echo "$i"
	   # or do whatever with individual element of the array
	done

	## or another way to iterate
	for  (( i=0; i<${#arr[@]}; i++ ));
	do
		echo $i ${arr[$i]}
	done
}

# iterate_number
# list_files
# iterate_output
iterate_array