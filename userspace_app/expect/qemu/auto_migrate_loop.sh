#!/bin/bash
source ~/.bashrc
counts=1
failures=0

for i in $(eval echo "{1..$counts}")
do
	printf "[%2d]: " $i
	expect -f ~/guest/auto_migrate ~/guest/qemu_command root 144025shyw &> .migrate_log

	if [ $? -ne 0 ]
	then
		echo Failed
		let failures++
	else
		echo Succeed
	fi
done

if [ $failures -ne 0 ]
then
	echo Migration failed $failures times out of $counts
else
	echo Migration Succeed for all $counts times
fi
