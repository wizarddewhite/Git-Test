#!/bin/bash

if [ $# != 2 ]; then
	echo $0 ip password
	exit -1
fi

/root/dup_machine/save_config.sh
echo userinfo updated

# setup remote key and copy it to remote
if [ ! -e /root/.ssh/id_rsa ]; then
	ssh-keygen -t rsa -f /root/.ssh/id_rsa -q -N ""
fi

/root/dup_machine/setup_key.exp $1 $2
val=$?
if [ $val -ne 0 ]; then
	echo failed to setup key
	exit -1
fi

# create dir
ssh -o "StrictHostKeyChecking no" root@$1 mkdir -p /root/dup_machine

# copy users.info to remote 
scp /root/dup_machine/users.info root@$1:/root/dup_machine/

# copy bandwith monitor to remote
scp -r /root/calculate_bandwidth root@$1:/root/

# copy restore file to remote
scp /root/dup_machine/restore_config.sh root@$1:/root/dup_machine/

# copy keys
keys=`find /home -name authorized_keys`
for k in $keys
do
	IFS='/' read -r -a array <<< "$k"
	scp $k root@$1:${array[2]}.key
done
#scp -r /home/ root@$1:/home/

# execute
echo do the stuff on remote
ssh root@$1 "/root/dup_machine/restore_config.sh &>/dev/null &"
