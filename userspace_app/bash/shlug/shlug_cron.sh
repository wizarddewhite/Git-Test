#!/bin/bash

# Add this line in /etc/crontab
#0 0     * * 2   root    /home/weiyang/go/shlug/shlug_cron.sh

cd /home/weiyang/go/shlug/script/
export JUHE_APPKEY="your_key"
./check.sh
result=$?
go run email.go $result
echo y | /home/weiyang/go/shlug/script/shlug_ann_h4.sh
