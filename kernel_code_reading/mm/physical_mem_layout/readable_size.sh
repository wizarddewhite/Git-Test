#!/bin/bash

UNIT=('B' 'K' 'M' 'G' 'T')
unit=''
uidx=0

is_int ()
{
	test "$@" -eq "$@" 2> /dev/null; 
}

usage()
{
	echo "Usage: make a size more readable"
	echo "$0 [-u <B|K|M|G>] size"
	exit
}

while getopts "hu:" opt; do
	case "$opt" in
	"h")
		usage
		;;
	"u")
		unit=${OPTARG}
		# check the unit
		for  (( uidx=0; uidx<${#UNIT[@]}; uidx++ ));
		do
			if [[ $unit == ${UNIT[$uidx]} ]]; then
				break
			fi
		done
		if [[ $uidx == ${#UNIT[@]} ]]; then
			usage
		fi
		;;
	":")
		echo "no argument for option: $OPTARG"
		;;
	"?")
		# this works when ":" is the first letter in optstring
		echo "not valid option: $OPTARG"
		usage
		;;
	esac
done
shift "$(($OPTIND - 1))"

if [ $# != 1 ]; then
	usage
fi

# check the parameter
if ! is_int $1 ; then
	echo "Error: should pass a valid integer as size"
	echo
	usage
fi

raw_size=$1
for ((i=$uidx;i<${#UNIT[@]};i++));
do
	size=$raw_size
	out=$raw_size
	for ((j=$uidx;j<i;j++));
	do
		size=`echo "$size / 1024" | bc`
		out=`echo "scale=3; $out / 1024" | bc`
	done

	if (( $size < 1024)) || [ $i == $(( ${#UNIT[@]} - 1 )) ]; then
		echo raw:   $1 $unit
		echo is : $out ${UNIT[$i]}
		break
	fi
done


