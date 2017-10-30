#/bin/bash

raw_bandwidth=`date +%Y%m%d%M`

# 10 minutes
nethogs -t -d 1 -c 600 | grep sshd > $raw_bandwidth

users=`awk '{print $2}' $raw_bandwidth | sort | uniq | awk -F"/" '{print $1}'`

for u in ${users[@]}
do
	awk -f bandwidth_calculation.awk user=$u $raw_bandwidth >> /home/$u/used
done

rm $raw_bandwidth
