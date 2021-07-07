#!/bin/bash

while getopts ":vp:" opt; do
	case "$opt" in
	"v")
		echo "get a"
		;;
	"p")
		echo "p's parameter is: $OPTARG"
		;;
	":")
		echo "no argument for option: $OPTARG"
		;;
	"?")
		echo "not valid option: $OPTARG"
		;;
	esac
done
