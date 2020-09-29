#!/bin/bash

cd /home/weiyang/go/shlug/script/
export JUHE_APPKEY="your_key"
./check.sh
result=$?
go run email.go $result
echo y | /home/weiyang/go/shlug/script/shlug_ann_h4.sh
