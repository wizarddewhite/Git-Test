#!/bin/bash

SETUP=false
UP=false
DOWN=false
STAT=false
OS=`uname`

conf=$HOME/.freeland.conf
keyfile=$HOME/.ssh/id_rsa
pub_keyfile=$HOME/.ssh/id_rsa.pub
proxy=""
port=1080

function firefox_config_check()
{
    if [ -z "$firefox_config" ]; then
        echo ""
        echo "You don't have firefox configuration file."
        echo "Install or Check your firefox first"
	exit
    fi
}

function firefox_process_check()
{
    if [ ! -z "$firefox_process" ]; then
        echo ""
	echo "Shutdown your firefox first"
	exit
    fi
}

function firefox_enable()
{
    firefox_config_check
    firefox_process_check
    echo "
        user_pref(\"network.proxy.socks\", \"localhost\");
        user_pref(\"network.proxy.socks_port\", 1080);
        user_pref(\"network.proxy.socks_remote_dns\", true);
        user_pref(\"network.proxy.type\", 1);
        " >> ${firefox_config[0]}
}

function firefox_disable()
{
    firefox_config_check
    firefox_process_check
    awk '!/proxy/' ${firefox_config[0]} > .mozilla_firefox_config.tmp
    mv .mozilla_firefox_config.tmp ${firefox_config[0]}
}

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
	    echo -n "Your MAC "
	    sudo networksetup -setsocksfirewallproxy Wi-Fi localhost $port
	    sudo networksetup -setsocksfirewallproxystate Wi-Fi on
	else
	    echo "Your " $OS " is not fully supported."
	    echo "Setup your proxy to localhost:$port manually"
	fi
	sleep 1
	ssh -M -S .freeland-control -fND $port $proxy -p 26 &> /dev/null
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
	    echo -n "Your MAC "
	    sudo networksetup -setsocksfirewallproxystate Wi-Fi off
	fi
	sleep 1
	ssh -S .freeland-control -O exit $proxy
	echo down!!!
	exit
fi

if [ "$STAT" = true ]; then
	read_conf
    	echo === stat of connection to $proxy
	ssh -S .freeland-control -O check $proxy
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
	ssh $username@$host -p 26 mkdir -p .ssh
	cat $pub_keyfile | ssh $username@$host -p 26 'cat >> .ssh/authorized_keys'
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
