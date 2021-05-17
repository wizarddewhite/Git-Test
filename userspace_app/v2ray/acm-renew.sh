stop bee
~/.acme.sh/acme.sh --issue -d freedomland.tk --standalone -k ec-256
~/.acme.sh/acme.sh --installcert -d freedomland.tk --fullchainpath /etc/v2ray/v2ray.crt --keypath /etc/v2ray/v2ray.key --ecc
service nginx restart
