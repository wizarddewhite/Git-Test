#!/bin/bash

# setup ssh config
echo "Port 26" >> /etc/ssh/sshd_config
echo "PasswordAuthentication no" >> /etc/ssh/sshd_config
echo "AllowUsers root" >> /etc/ssh/sshd_config
#echo "ObfuscateKeyword $1" >> /usr/local/obfus/etc/sshd_config
#echo "ObfuscatedPort $2" >> /usr/local/obfus/etc/sshd_config
usermod -g root -G ssh root
service sshd restart
#mkdir /var/empty
#/usr/local/obfus/sbin/sshd -f /usr/local/obfus/etc/sshd_config &

# install nethogs
#apt-get -y install nethogs

# enable tcp brr
echo 'net.core.default_qdisc=fq' | tee -a /etc/sysctl.conf
echo 'net.ipv4.tcp_congestion_control=bbr' | tee -a /etc/sysctl.conf
sysctl -p

# setup server
cp /root/server/v2ray.service /etc/systemd/system/v2ray.service
cp /root/server/v2ray.service /etc/systemd/system/multi-user.target.wants/v2ray.service
mkdir -p /usr/bin/v2ray
cp /root/server/v2ray /usr/bin/v2ray/v2ray
cp /root/server/v2ctl /usr/bin/v2ray/v2ctl
mkdir -p /etc/v2ray
cp /root/server/config.json /etc/v2ray/config.json
cp /root/server/v2ray.crt /etc/v2ray/v2ray.crt
cp /root/server/v2ray.key /etc/v2ray/v2ray.key
mkdir -p /var/log/v2ray

# install and setup nginx
apt-get -y install nginx
cp /root/server/nginx.conf /etc/nginx/nginx.conf
cp /root/server/nginx-default /etc/nginx/sites-available/default
service nginx restart

# start server
service v2ray start

# restore users
# unpack key file
#tar -xzvf keys.tar.gz -C /
#while read user
#do
#	IFS=' ' read -r -a array <<< "$user"
#	# create user
#	useradd -s /bin/true -d /home/${array[0]} -m ${array[0]} &> /dev/null
#	usermod -p '*' ${array[0]}
#	# create .ssh dir
#	mkdir -p /home/${array[0]}/.ssh
#	# get key file
#	mv /root/keys/${array[0]}.key /home/${array[0]}/.ssh/authorized_keys
#	# chown
#	chown -R ${array[0]}:${array[0]} /home/${array[0]}/.ssh
#	# add to ssh if enabled
#	if [ ${array[1]} -ne 0 ]; then
#		usermod -g ${array[0]} -G ssh ${array[0]}
#	fi
#done < /root/dup_machine/users.info

# deploy bandwidth calculation
#echo " */10    *  * * *   root    cd /root/calculate_bandwidth && ./get_user_bandwidth.sh " >> /etc/crontab
cp /root/calculate_bandwidth/v2ray_bandwidth.service /etc/systemd/system/multi-user.target.wants/v2ray_bandwidth.service
cp /root/calculate_bandwidth/v2ray_bandwidth.service /etc/systemd/system/v2ray_bandwidth.service
service v2ray_bandwidth start

# mark done
echo " -Z $1 -p $2" > /root/done
