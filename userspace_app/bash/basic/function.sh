#!/bin/bash

function f1() 
{
	prune="ywtest f1"
	local local_prune="ywtest local"
	echo "this is function 1"
}

f1

function f2()
{
	echo "this is function 2"
	echo $# args: $*
	echo '$0 is always the file name' $0
	echo the first arg is \"$1\"
	echo 'prune is' $prune
	echo 'local_prune is' $local_prune
	return 2
}

f2 a b 
echo return value of f2 is: $?
