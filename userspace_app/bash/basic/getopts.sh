#!/bin/bash

path=""

# getopts optstring name
while getopts "vp:" opt; do
	case "$opt" in
	"v")
		echo "get option v"
		;;
	"p")
		echo "p's parameter is: $OPTARG"
		path=${OPTARG}
		;;
	":")
		echo "no argument for option: $OPTARG"
		;;
	"?")
		# this works when ":" is the first letter in optstring
		echo "not valid option: $OPTARG"
		;;
	esac
done

if [ -z "${path}" ] ; then
    echo path is a must
fi

