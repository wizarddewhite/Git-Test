#!/usr/bin/env bash

next_thursday() {
    fmt=$1
    for i in `seq 1 7`; do
        day="$(date +%F) +${i} days"
        if [ X$(date -d "$day" +%w) = X4 ]; then
            date -d "$day" "+${fmt}"
        fi
    done
}

h4_month=`next_thursday "%Y-%-m"`
h4_date=`next_thursday "%Y-%-m-%-d"`

# check holiday in list first
while IFS= read -r line; do
    #echo "Text read from file: $line"
    if [ `date -d "$line" '+%Y-%-m-%-d'` = "$h4_date" ]; then
        echo 1
        exit 1
    fi
done < cancel_list

: ${JUHE_APPKEY:?"Need to set JUHE_APPKEY"}
url="http://v.juhe.cn/calendar/month?year-month="
url="$url$h4_month&key=$JUHE_APPKEY"
json=`curl -s $url | jq -rf jq_filter.jq 2>/dev/null`
if [ $? -ne 0 ]
then
	echo 0
	exit
fi

echo $json | grep -w $h4_date &> /dev/null
is_holiday=`echo $?`

if [ "X$is_holiday" = X0 ]
then
    echo $h4_date is holiday
    exit 2
fi

echo $h4_date is not holiday
exit 0
