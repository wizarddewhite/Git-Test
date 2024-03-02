#!/bin/bash

path=""

usage()
{
	echo "Usage: embed subtitle to video"
	echo "       $0 [-h] -b base_name"
	echo 
	echo "       Put video and subtitle(en/chn) in the same directory with same base name."
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
subtitle_en=${basename}_en.srt
subtitle_chn=${basename}_chn.srt
final=${basename}_final.mp4
subtitle_merge=${basename}_merge.srt

if [[ ! -f "${video}" || ! -f "${subtitle_en}" || ! -f "${subtitle_chn}" ]]; then
	echo [ERROR] ${video} or ${subtitle_en} or ${subtitle_chn} not exist
	echo
	usage
fi

cat > ${subtitle_merge} << EOF
1
00:00:00,000 --> 00:00:09,000
视频转录&字幕合成：杨小伟的世界

EOF

# cleanup the google translation result
sed -i -E "s/第([0-9].*)章/\1/" ${subtitle_chn}
sed -i -E "s/^([0-9].*)年$/\1/"  ${subtitle_chn}

cat ${subtitle_en} >> ${subtitle_merge}
cat ${subtitle_chn} >> ${subtitle_merge}

rm -f ${final}
ffmpeg -i ${video} -vf subtitles=${subtitle_merge} ${final}
# rm -f ${subtitle_merge}

# get duration
# ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 -i ${final}
