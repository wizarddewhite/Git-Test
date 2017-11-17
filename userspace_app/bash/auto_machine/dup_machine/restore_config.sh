#!/bin/bash

# setup ssh config
echo "Port 26" >> /etc/ssh/sshd_config
echo "AllowGroups ssh" >> /etc/ssh/sshd_config
service sshd restart

# install nethogs
apt-get -y install nethogs

# enable tcp brr
echo 'net.core.default_qdisc=fq' | tee -a /etc/sysctl.conf
echo 'net.ipv4.tcp_congestion_control=bbr' | tee -a /etc/sysctl.conf
sysctl -p

# restore users
while read user
do
	IFS=' ' read -r -a array <<< "$user"
	# create user
	useradd -s /bin/true -d /home/${array[0]} -m ${array[0]} &> /dev/null
	# create .ssh dir
	mkdir -p /home/${array[0]}/.ssh
	# add to ssh if enabled
	if [ ${array[1]} -ne 0 ]; then
		usermod -g ${array[0]} -G ssh ${array[0]}
	fi
done < $PWD/users.info

# deploy bandwidth calculation
echo " */10    *  * * *   root    cd /root/calculate_bandwidth && ./get_user_bandwidth.sh " >> /etc/crontab
