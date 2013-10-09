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
