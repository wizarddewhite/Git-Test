#!/bin/bash

echo "from 10 to 19"
for ((i=10;i<20;i++));
do
	if [ $i == 15 ];
	then
		break;
	fi
	echo "  " $i;
done

# or and
if [[ 1 > 0 && -1 < 7 ]]; then
	echo "both meet"
else
	echo "one not meet"
fi

if [[ 1 > 0 || 17 < 7 ]]; then
	echo "one meet"
else
	echo "non meet"
fi

# check file existence
if [ ! -f "file_non_exist" ]; then
	echo "file not exist"
else
	echo "file exist"
fi

# string compare
if [ "string1" == "string2" ]; then
	echo "Equal Strings"
else
	echo "Strings not equal"
fi

if [ -z "" ]; then
	echo "Empty String"
else
	echo "Non-Empty String"
fi

if [ -n "" ]; then
	echo "Non-Empty String"
else
	echo "Empty String"
fi
