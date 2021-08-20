#!/bin/bash

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

# https://stackoverflow.com/questions/15685736/how-to-extract-a-particular-element-from-an-array-in-bash 
function array_split()
{
	myarr=('hello big world!' 'how are you' 'where am I')
	# Entire first element of the array
	echo "origin: ${myarr[0]}"

	# To get the last word, remove prefix through the last space
	echo "last: ${myarr[0]##* }"

	# To get the first word, remove suffix starting with the first space
	echo "${myarr[0]%% *}"

	# The second word is harder; first remove through the first space...
	tmp="${myarr[0]#* }"
	# ...then get the first word of what remains
	echo "${tmp%% *}"

	# The third word (which might not be the last)? remove through the second space...
	tmp="${myarr[0]#* * }"
	# ...then the first word again
	echo "${tmp%% *}"
}

function remove_last()
{
	arr=('hello' 'big' 'world!')
	echo "origin: ${arr[@]}"
	unset 'arr[${#arr[@]}-1]'
	echo "left: ${arr[@]}"
}

# iterate_array
# array_split
remove_last
