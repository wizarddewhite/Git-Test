#!/bin/bash

SETUP=false
UP=false
DOWN=false
STAT=false

conf=$HOME/.freeland.conf
keyfile=$HOME/.ssh/id_rsa
pub_keyfile=$HOME/.ssh/id_rsa.pub
proxy=""
port=1080

function usage() 
{
    echo "Usage: $0 [setup|up|down|stat]"
    echo "Before using, run \"$0 setup\" first to prepare the environment."
    echo " "
    echo "setup: upload key and setup your account"
    echo "up:    create connection"
    echo "down:  destroy connection"
    echo "stat:  show connection state"
    exit 0
}

function read_conf()
{
    # get config
    if [ ! -e "$conf" ]; then
    	echo run \"$0 setup\" first
    	exit
    fi
    
    while read line
    do
        proxy=$line
    done < $conf
}

if [ $# != 1 ]; then
	usage
fi

# parse parameter
if [ "$1" == "setup" ]; then
	SETUP=true
elif [ "$1" == "up" ]; then
	UP=true
elif [ "$1" == "down" ]; then
	DOWN=true
elif [ "$1" == "stat" ]; then
	STAT=true
else
	usage
fi

if [ "$UP" = true ]; then
	read_conf
    	echo making connection to $proxy
	ssh -M -S .freeland-control -fND $port $proxy
	sleep 1
	networksetup -setsocksfirewallproxy Wi-Fi localhost $port
	sudo networksetup -setsocksfirewallproxystate Wi-Fi on
	echo UP!!!
	exit
fi

if [ "$DOWN" = true ]; then
	read_conf
	sudo networksetup -setsocksfirewallproxystate Wi-Fi off
	sleep 1
	ssh -S .freeland-control -O exit $proxy
	echo $proxy connection down!!!
	exit
fi

if [ "$STAT" = true ]; then
	read_conf
    	echo === stat of connection to $proxy
	ssh -S .freeland-control -O check $proxy
    	echo === stat of SOCKS proxy
	networksetup -getsocksfirewallproxy Wi-Fi
	exit
fi

if [ ! -e "$pub_keyfile" ]; then
	ssh-keygen -t rsa -f $keyfile -q -N ""
fi

if [ "$SETUP" = true ]; then
	echo -n "username:"
	read username
	echo -n "host ip:"
	read host
	ssh $username@$host mkdir -p .ssh
	cat $pub_keyfile | ssh $username@$host 'cat >> .ssh/authorized_keys'
	val=$?
	if [ $val -ne 0 ]; then
		echo 
		echo Error!!! setup failed
		echo Please check your username/host ip/password
		exit
	fi

	echo $username@$host > $conf
	echo done
fi
