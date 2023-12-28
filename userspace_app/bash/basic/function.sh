#!/bin/bash

function f1() 
{
	echo "this is function 1"
}

f1

function f2()
{
	echo "this is function 2"
	echo $# args: $*
	echo '$0 is always the file name' $0
	echo the first arg is \"$1\"
	return 2
}

f2 a b 
echo return value of f2 is: $?
