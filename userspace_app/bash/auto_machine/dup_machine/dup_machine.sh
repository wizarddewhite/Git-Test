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

sshpass -p $2 ssh -o "StrictHostKeyChecking no" root@$1 mkdir -p .ssh
sshpass -p $2 scp /root/.ssh/id_rsa.pub root@$1:/root/.ssh/authorized_keys

# create dir
ssh -o "StrictHostKeyChecking no" root@$1 mkdir -p /root/dup_machine

# copy users.info to remote 
scp /root/dup_machine/users.info root@$1:/root/dup_machine/

# copy bandwith monitor to remote
scp -r /root/calculate_bandwidth root@$1:/root/

# copy restore file to remote
scp /root/dup_machine/restore_config.sh root@$1:/root/dup_machine/

# copy keys
users=`ls /home/`
for u in $users
do
	cp /home/$u/.ssh/authorized_keys /root/keys/$u.key
done
tar -zcvf /root/keys.tar.gz /root/keys/
scp /root/keys.tar.gz root@$1:
rm -f /root/keys.tar.gz /root/keys/*

# execute
echo do the stuff on remote
ssh root@$1 "/root/dup_machine/restore_config.sh &>/dev/null &"
