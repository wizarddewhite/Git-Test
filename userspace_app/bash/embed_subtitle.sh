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
subtitle_merge=${basename}_merge.srt

if [[ ! -f "${video}" || ! -f "${subtitle}" ]]; then
	echo [ERROR] ${video} or ${subtitle} not exist
	echo
	usage
fi

cat > ${subtitle_merge} << EOF
1
00:00:00,000 --> 00:00:09,000
视频转录&字幕合成：杨小伟的世界

EOF

cat ${subtitle} >> ${subtitle_merge}

rm -f ${final}
ffmpeg -i ${video} -vf subtitles=${subtitle_merge} ${final}
rm -f ${subtitle_merge}
