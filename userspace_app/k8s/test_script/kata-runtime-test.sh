#!/bin/bash

count=$1
pod_name_prefix="kata-runtime-test"
failures=0

function wait_running()
{
	for i in `seq 0 30`; do
		sleep 2
		status=`kubectl get pods $1 -o jsonpath='{.status.phase}'`
		[[ "$status" == "Running" ]] && echo $1 is Running && return 0
		echo $1 is $status
	done

	echo $1 failed run
	return 1
}

function test_kata() 
{
	echo Start $1
	# generate pod yaml and apply it
	./generate_pod_yaml.sh $1 > yamls/$1.yaml
	kubectl apply -f yamls/$1.yaml &> /dev/null

	# check pod status to RUNNING
	wait_running $1
	[[ $? -eq "1" ]] && return 1

	# remove pod
	echo remove pod $1
	kubectl delete -f yamls/$1.yaml &> /dev/null
}

while :
do
	let "failures = 0"
	for (( i = 0; $i < $count; i++ )) {
		test_kata $pod_name_prefix$i &
	}

	for job in `jobs -p`
	do
		wait $job
		res=$?
		[[ $res -eq "1" ]] && let "failures += 1"
	done
	[[ $failures -ne "0" ]] && echo $failures failed && exit
	echo Test all pass
done
