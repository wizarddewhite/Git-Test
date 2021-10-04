#!/bin/bash
save=0
while getopts ":s" opt; do
	case "$opt" in
	"s")
		save=1
		;;
	"?")
		echo "not valid option: $OPTARG"
		;;
	esac
done
shift $((OPTIND-1))

if [ $# -eq 0 ]; then
    echo "You should specify the patch file"
    exit
fi

./scripts/get_maintainer.pl $* 2> /dev/null | awk -F'[<>]' '/</ {print "--to " $2}' > .tmp_maintainer
./scripts/get_maintainer.pl $* 2> /dev/null | awk '!/</ {print "--cc " $1}' >> .tmp_maintainer

if [ $save -eq 0 ]; then
	cat .tmp_maintainer | while read line
	do
		printf "%s" "$line "
	done
else
	rm -f email_receiver
	touch email_receiver
	cat .tmp_maintainer | while read line
	do
		printf "%s" "$line " >> email_receiver
	done
fi

rm .tmp_maintainer
