#!/bin/bash

if [ $# != 4 ]; then
	echo $0 ip password word port
	exit -1
fi

# setup remote key and copy it to remote
if [ ! -e /root/.ssh/id_rsa ]; then
	ssh-keygen -t rsa -f /root/.ssh/id_rsa -q -N ""
fi

sshpass -p "$2" ssh -o "StrictHostKeyChecking no" root@$1 mkdir -p .ssh
sshpass -p "$2" scp /root/.ssh/id_rsa.pub root@$1:/root/.ssh/authorized_keys

# create dir
ssh -o "StrictHostKeyChecking no" root@$1 mkdir -p /root/dup_machine

# copy bandwith monitor to remote
scp -r /root/calculate_bandwidth root@$1:/root/
scp -r /root/tasks root@$1:/root/

# copy other server config
mkdir -p /root/server
cp /etc/nginx/nginx.conf /root/server/
cp /etc/nginx/sites-available/default /root/server/nginx-default
cp /etc/systemd/system/v2ray.service /root/server/
cp /usr/bin/v2ray/v2ray /root/server/
cp /usr/bin/v2ray/v2ctl /root/server/
cp /etc/v2ray/config.json /root/server/
cp /etc/v2ray/v2ray.crt /root/server/
cp /etc/v2ray/v2ray.key /root/server/
scp -r /root/server root@$1:/root/

# copy restore file to remote
scp /root/dup_machine/restore_config.sh root@$1:/root/dup_machine/

# execute
echo do the stuff on remote
ssh root@$1 "/root/dup_machine/restore_config.sh $3 $4 &>/dev/null &"