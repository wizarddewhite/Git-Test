~/.acme.sh/acme.sh --issue -d freedom.tk --standalone -k ec-256
~/.acme.sh/acme.sh --installcert -d freedom.tk --fullchainpath /etc/v2ray/v2ray.crt --keypath /etc/v2ray/v2ray.key --ecc
stop bee
service nginx restart
