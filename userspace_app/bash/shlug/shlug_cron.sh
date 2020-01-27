#!/bin/bash

cd /home/weiyang/go/shlug/script/
export JUHE_APPKEY="your_key"
go run email.go `./check.sh`
echo y | /home/weiyang/go/shlug/script/shlug_ann_h4.sh
