#!/bin/bash

is_int ()
{
	test "$@" -eq "$@" 2> /dev/null; 
}

usage()
{
	echo "Usage: convert page number to readable size"
	echo $0 page_number
	exit
}

while getopts "h" opt; do
	case "$opt" in
	"h")
		usage
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

if [ $# != 1 ]; then
	usage
fi

if ! is_int $1 ; then
	echo "Error: should pass a valid integer for page number"
	echo
	usage
fi

echo "#page: " $1

UNIT=('K' 'M' 'G' 'P')
raw_size=`echo "$1 * 4" | bc`
echo size : $raw_size K

for ((i=1;i<10;i++));
do
	size=$raw_size
	out=$raw_size
	for ((j=1;j<i;j++));
	do
		size=`echo "$size / 1024" | bc`
		out=`echo "scale=3; $out / 1024" | bc`
	done

	if (( $size < 1024)); then
		echo equal: $out ${UNIT[$i - 1]}
		break
	fi
done

