#!/bin/bash 

server="pop.gmail.com:995"

fp=`openssl s_client -connect $server -showcerts < /dev/null 2> /dev/null \
	| openssl x509 -fingerprint -md5 -text | grep Fingerprint \
	| awk -F"=" '{print $2}'`
echo $fp

sed -i "s/sslfingerprint.*/sslfingerprint '$fp'/" ~/.fetchmailrc
