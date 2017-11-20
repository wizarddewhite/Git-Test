#/bin/bash
info=/root/dup_machine/users.info

rm -f $info
cd /home && USERS=`ls`

for u in $USERS
do
	id $u | grep ssh &> /dev/null
	val=$?
	if [ $val -ne 0 ]; then
		echo $u " " 0 >> $info
	else
		echo $u " " 1 >> $info
	fi
done
