#!/bin/bash

SETUP=false
UP=false
DOWN=false
STAT=false
OS=`uname`

conf=$HOME/.freeland.conf
keyfile=$HOME/.ssh/id_rsa
pub_keyfile=$HOME/.ssh/id_rsa.pub
sess_con=$HOME/.freeland-control
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
	if [ $OS == "Darwin" ]; then
	    sudo networksetup -setsocksfirewallproxy Wi-Fi localhost $port
	    sudo networksetup -setsocksfirewallproxystate Wi-Fi on
	else
	    echo "Your " $OS " is not fully supported."
	    echo "Setup your proxy to localhost:$port manually"
	fi
	sleep 1
	if [ -e "$sess_con" ]; then
	    ssh -S $sess_con -O exit $proxy
	fi
	ssh -o "StrictHostKeyChecking no" -M -S $sess_con -fND $port $proxy -p 26 &> /dev/null
	val=$?
	if [ $val -ne 0 ]; then
	    echo Failed!!!
	else
	    echo UP!!!
	fi
	exit
fi

if [ "$DOWN" = true ]; then
	read_conf
	if [ $OS == "Darwin" ]; then
	    sudo networksetup -setsocksfirewallproxystate Wi-Fi off
	fi
	sleep 1
	ssh -S $sess_con -O exit $proxy
	echo down!!!
	exit
fi

if [ "$STAT" = true ]; then
	read_conf
    	echo === stat of connection to $proxy
	ssh -S $sess_con -O check $proxy
	echo === stat of SOCKS proxy
	if [ $OS == "Darwin" ]; then
	    networksetup -getsocksfirewallproxy Wi-Fi
	fi
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

	echo $username@$host > $conf
	if [ $OS == "Darwin" ]; then
		echo -n "Your MAC "
		sudo sh -c "echo '$USER ALL=(root) NOPASSWD:/usr/sbin/networksetup' >> /etc/sudoers"
	fi
	echo ""
	echo copy following characters between lines to your account:
	echo ----------------------------------
	cat $pub_keyfile
	echo ----------------------------------
	echo done
fi
