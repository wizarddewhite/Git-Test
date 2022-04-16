#!/bin/bash

path=""

usage()
{
	echo "Usage: example on getopts"
	echo $0 [-h] [-v] -p path
	exit
}

# getopts optstring name
while getopts "hvp:" opt; do
	case "$opt" in
	"h")
		usage
		;;
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

