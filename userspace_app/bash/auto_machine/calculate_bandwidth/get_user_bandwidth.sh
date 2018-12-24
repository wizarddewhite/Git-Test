#!/bin/bash

raw_bandwidth=`date +%Y%m%d%H%M`

# 10 minutes
nethogs -d 60 -c 10 -t -v 3 > $PWD/$raw_bandwidth
tail -n +`grep -n Refreshing: $PWD/$raw_bandwidth | tail -1 | cut -d ":" -f 1` $PWD/$raw_bandwidth > $PWD/$raw_bandwidth.tmp

users=`awk '{print $2}' $PWD/$raw_bandwidth.tmp | awk -F"/" '{print $1}' | sort | uniq`

touch /root/total_used
for u in $users
do
	if [ -e /home/$u ]; then
	    awk -f $PWD/bandwidth_calculation.awk user=$u $PWD/$raw_bandwidth.tmp >> /root/total_used
	fi
done

ps auxwww | grep sshd: | awk '{print $1}' | sort | uniq | grep -v root | wc -l > /root/sessions

$PWD/json_post > /dev/null

rm $PWD/$raw_bandwidth $PWD/$raw_bandwidth.tmp /root/total_used /root/sessions

