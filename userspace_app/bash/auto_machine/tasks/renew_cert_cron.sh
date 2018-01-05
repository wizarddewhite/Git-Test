#!/bin/bash
# a monthly crontab
# 0 0 1 * * /root/tasks/renew_cert_cron.sh
/root/.acme.sh/acme.sh --renew -d freedomland.tk --force --ecc
/root/.acme.sh/acme.sh --installcert -d freedomland.tk --fullchainpath /etc/v2ray/v2ray.crt --keypath /etc/v2ray/v2ray.key --ecc
service nginx restart
sleep 5
curl http://127.0.0.1/node/renew_k

