#!/bin/bash

path=""

usage()
{
	echo "Usage: example on getopts"
	echo "$0 [-h] [-v] [-s <45|90>] -p path"
	exit
}

# getopts optstring name
while getopts ":hvp:s:" opt; do
	case "$opt" in
	"h")
		usage
		;;
	"s")
		s=${OPTARG}
		((s == 45 || s == 90)) || usage
		;;
	"v")
		echo "get option v"
		;;
	"p")
		echo "p's parameter is: $OPTARG"
		path=${OPTARG}
		;;
	":")
		# this works when ":" is the first letter in optstring
		echo "no argument for option: $OPTARG"
		usage
		;;
	"?")
		echo "not valid option: $OPTARG"
		usage
		;;
	esac
done

if [ -z "${path}" ] ; then
    echo path is a must
fi

shift "$(($OPTIND - 1))"
printf "Remaining arguments are: %s\n" "$*"
