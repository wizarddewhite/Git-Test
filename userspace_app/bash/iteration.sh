#!/bin/bash
# increase one on each boot

if [ ! -f "/home/weiyang/.reboot_iteration" ]; then
	echo 0 > /home/weiyang/.reboot_iteration
fi

iteration=`cat /home/weiyang/.reboot_iteration`
echo $iteration
echo $(($iteration + 1)) > /home/weiyang/.reboot_iteration

