# git clone https://github.com/acmesh-official/acme.sh.git
# ./acme.sh --install -m shyw13@gmail.com
# cd git/acme.sh
./acme.sh --issue -d freedomland.tk --standalone -k ec-256
./acme.sh --installcert -d freedomland.tk --fullchainpath /etc/v2ray/v2ray.crt --keypath /etc/v2ray/v2ray.key --ecc
stop bee
service nginx restart
