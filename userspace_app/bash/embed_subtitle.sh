#!/bin/bash

path=""

usage()
{
	echo "Usage: embed subtitle to video"
	echo "       $0 [-h] -b base_name"
	echo 
	echo "       Put video and subtitle in the same directory with same base name."
	echo "       Then execute the script."
	exit
}

# getopts optstring name
while getopts ":hb:" opt; do
	case "$opt" in
	"h")
		usage
		;;
	"b")
		basename=${OPTARG}
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

if [ -z "${basename}" ] ; then
    usage
fi

video=${basename}.mp4
subtitle=${basename}.srt
final=${basename}_final.mp4

if [[ ! -f "${video}" || ! -f "${subtitle}" ]]; then
	echo [ERROR] ${video} or ${subtitle} not exist
	echo
	usage
fi


ffmpeg -i ${video} -vf subtitles=${subtitle} ${final}
