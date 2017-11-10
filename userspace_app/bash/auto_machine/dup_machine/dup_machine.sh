#!/bin/bash

if [ $# != 2 ]; then
	echo $0 ip password
	exit -1
fi

$PWD/save_config.sh
echo userinfo updated

# setup remote key and copy it to remote
if [ ! -e /root/.ssh/id_rsa ]; then
	ssh-keygen -t rsa -f /root/.ssh/id_rsa -q -N ""
fi

$PWD/setup_key.exp $1 $2
val=$?
if [ $val -ne 0 ]; then
	echo failed to setup key
	exit -1
fi

# create dir
ssh -o "StrictHostKeyChecking no" root@$1 mkdir -p $PWD

# copy users.info to remote 
scp $PWD/users.info root@$1:$PWD/

# copy bandwith monitor to remote
scp -r /root/calculate_bandwidth root@$1:/root/

# copy restore file to remote
scp $PWD/restore_config.sh root@$1:$PWD/
# execute
echo do the stuff on remote
ssh root@$1 "cd $PWD; ./restore_config.sh"

# copy keys
keys=`find /home -name authorized_keys`
for k in $keys
do
	scp $k root@$1:$k
done
#scp -r /home/ root@$1:/home/

