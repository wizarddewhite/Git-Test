#/bin/bash

raw_bandwidth=`date +%Y%m%d%H%M`

# 10 minutes
nethogs -d 60 -c 10 -t -v 3 > /root/$raw_bandwidth
tail -n +`grep -n Refreshing: /root/$raw_bandwidth | tail -1 | cut -d ":" -f 1` /root/$raw_bandwidth > /root/$raw_bandwidth.tmp

users=`awk '{print $2}' $raw_bandwidth.tmp | sort | uniq | awk -F"/" '{print $1}'`

for u in $users
do
	if [ -e /home/$u ]; then
	    awk -f bandwidth_calculation.awk user=$u /root/$raw_bandwidth.tmp >> /root/total_used
	fi
done

/root/json_post > /dev/null

rm /root/$raw_bandwidth /root/$raw_bandwidth.tmp /root/total_used


