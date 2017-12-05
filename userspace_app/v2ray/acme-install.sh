#!/bin/bash

apt install socat
curl  https://get.acme.sh | sh
source ~/.bashrc
~/.acme.sh/acme.sh --issue -d weiyang.ga --standalone -k ec-256
~/.acme.sh/acme.sh --installcert -d weiyang.ga --fullchainpath /etc/v2ray/v2ray.crt --keypath /etc/v2ray/v2ray.key --ecc
