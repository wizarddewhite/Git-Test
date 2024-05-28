#!/bin/bash

UNIT=('B' 'K' 'M' 'G' 'T')
unit='K'
uidx=1

just_result=""

is_int ()
{
	test "$@" -eq "$@" 2> /dev/null; 
}

usage()
{
	echo "Usage: pass a size and return a more readable format"
	echo "       Default unit is K"
	echo "$0 [-j] [-u <B|K|M|G>] size"
	echo "   -j just print result"
	echo "   -u the unit of input value"
	exit
}

while getopts "hju:" opt; do
	case "$opt" in
	"h")
		usage
		;;
	"j")
		# just print result
		just_result="y"
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
		# exit if unit not supported
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

# convert input value to B
input_size=$1
for ((i=$uidx;i>0;i--));
do
	input_size=`echo "$input_size * 1024" | bc`
done

size=$input_size
out=$input_size

for ((i=0;i<${#UNIT[@]};i++));
do
	# return if this level size is less than 1024 or hit highest unit
	if (( $size < 1024)) || [ $i == $(( ${#UNIT[@]} - 1 )) ]; then
		if [ -z $just_result ]; then
			echo raw:   $1 $unit
			echo is : $out ${UNIT[$i]}
		else
			echo $out ${UNIT[$i]}
		fi
		break
	fi

	size=`echo "$size / 1024" | bc`
	out=`echo "scale=3; $out / 1024" | bc`
done
