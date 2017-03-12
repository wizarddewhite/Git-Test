#!/bin/bash

sleep 1

if [ ! -f "/home/weiyang/.reboot_iteration" ]; then
	echo 0 > /home/weiyang/.reboot_iteration
fi

iteration=`cat /home/weiyang/.reboot_iteration`

if [ $iteration -le 6 ]; then
	reboot
fi

