#!/bin/bash
scp -P 26 /etc/v2ray/v2ray.crt root@$1:/etc/v2ray/v2ray.crt
scp -P 26 /etc/v2ray/v2ray.key root@$1:/etc/v2ray/v2ray.key
ssh root@$1 -p 26 "service nginx restart"
