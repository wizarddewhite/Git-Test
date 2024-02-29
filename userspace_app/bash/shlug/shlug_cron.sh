#!/bin/bash

# Add this line in /etc/crontab
#0 0     * * 2   root    /home/weiyang/go/shlug/shlug_cron.sh

touch /root/shlug_notify
date >> /root/shlug_notify
cd /home/weiyang/go/shlug/script/
export JUHE_APPKEY="your_key"
./check.sh
result=$?
echo "result is $?" >> /root/shlug_notify
go run email.go $result
echo y | /home/weiyang/go/shlug/script/shlug_ann_h4.sh
echo "=========" >> /root/shlug_notify

