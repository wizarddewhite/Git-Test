if [ $# != 2 ]
then
	echo "Usage: $0 raw_data_ori raw_data_opt"
	exit
fi

# calculate average static
awk -f time_parse.awk $1 > .avg_ori
awk -f time_parse.awk $2 > .avg_opt

# compare the orig and opt
awk -f compare.awk .avg_ori .avg_opt

# remove temp file
rm -f .avg_ori .avg_opt
