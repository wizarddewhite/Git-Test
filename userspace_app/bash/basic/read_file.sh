#!/bin/bash

cat machine_list |  while  read LINE   
do 
	echo  $LINE
	machine=(${LINE})
	echo node: ${machine[0]}  id: ${machine[1]}
done

