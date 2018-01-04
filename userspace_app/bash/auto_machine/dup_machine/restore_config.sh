#!/bin/bash

# setup ssh config
echo "Port 26" >> /etc/ssh/sshd_config
echo "PasswordAuthentication no" >> /etc/ssh/sshd_config
echo "AllowUsers root" >> /etc/ssh/sshd_config
usermod -g root -G ssh root
service sshd restart


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

# deploy bandwidth calculation
#echo " */10    *  * * *   root    cd /root/calculate_bandwidth && ./get_user_bandwidth.sh " >> /etc/crontab
cp /root/calculate_bandwidth/v2ray_bandwidth.service /etc/systemd/system/multi-user.target.wants/v2ray_bandwidth.service
cp /root/calculate_bandwidth/v2ray_bandwidth.service /etc/systemd/system/v2ray_bandwidth.service
service v2ray_bandwidth start

# mark done
echo " -Z $1 -p $2" > /root/done
