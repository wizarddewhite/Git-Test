#!/bin/bash

pod_name_prefix="kata-runtime-test"
failures=0
declare -a supported_cases=(
fio
sysbench
iperf
)

function usage()
{
	echo "Usage: $0 -p parallel -n numbers -c case1,case2"
	echo " "
	echo "-p:  specify parallel"
	echo "-n:  specify number of turns tests"
	echo "-c:  specify test cases to run with comma separated list"
	echo " "
	echo "Current supported cases are:"
	for (( j=0; j<${#supported_cases[@]}; j++ ))
	do
		echo ${supported_cases[$j]}
	done
	exit 0
}

function wait_running()
{
	for i in `seq 0 60`; do
		status=`kubectl get pods $1 | grep $1 | awk '{print $3}'`
		sleep 2
		[[ "$status" == "Running" ]] && echo $1 is Running after 2*$i seconds && return 0
		echo $1 is $status ... $i
	done

	echo $1 failed to be Running in 2min | tee -a result
	return 1
}

function test_fio()
{
	echo === running fio for $1
	kubectl exec $1 -- fio -ioengine=psync -direct=1 -bs=4k -time_based=1 -thread -rw=randrw \
	-percentage_random=100,0 -rwmixwrite=20 -size=1g \
	-filename=/data0/check_hardware.fio \
	-name="fio mixed randread and sequential write test" \
	-iodepth=1 -runtime=30 -numjobs=4 -group_reporting
	return $?
}

function test_sysbench()
{
	echo === running sysbench for $1
	kubectl exec $1 -- sysbench memory run
	return $?
}

function test_iperf()
{
	echo === running iperf for $1
	kubectl exec $1 -- iperf -c 10.19.22.23 -i 1 -t 10
	return $?
}

function test_kata()
{
	echo Start $1
	# generate pod yaml and apply it
	./generate_pod_yaml.sh $1 > yamls/$1.yaml
	kubectl apply -f yamls/$1.yaml &> /dev/null
	# check pod status to RUNNING
	wait_running $1 &>> results/$1.result
	[[ $? -eq "1" ]] && return 1

	# run cases user specified
	for (( i=0; i<${#cases[@]}; i++ ))
	do
		echo Test ${cases[$i]} on $1
		test_${cases[$i]} $1 &>> results/$1.result
		[[ $? -ne "0" ]] && echo "test_${cases[$i]} $1 failed" | tee -a result && return 1
	done

	return 0
}

rm -f result
mkdir -p results
rm -f results/*
mkdir -p yamls
rm -f yamls/*
parallel=1
numbers=1

while getopts ":p:n:c:" opt; do
	case "$opt" in
	"p")
		parallel=${OPTARG}
		;;
	"n")
		numbers=${OPTARG}
		;;
	"c")
		cases=${OPTARG}
		;;
	":")
		echo "no argument for option: $OPTARG"
		;;
	"?")
		# this works when ":" is the first letter in optstring
		echo "not valid option: $OPTARG"
		;;
	esac
done

IFS=, read -a cases <<< "${cases}"
# Do we support this case?
for (( i=0; i<${#cases[@]}; i++ ))
do
	for (( j=0; j<${#supported_cases[@]}; j++ ))
	do
		# support break
		if [ ${cases[$i]} == ${supported_cases[$j]} ]; then
			break
		fi
	done

	# still not find supported case?
	if [ "$j" -ge "${#supported_cases[@]}" ]; then
		echo ${cases[i]} is not supported
		usage
	fi
done

for (( j = 0; $j < $numbers; j++ )) {
	let "failures = 0"
	echo === Starting test for round $j
	for (( i = 0; $i < $parallel; i++ )) {
		test_kata $pod_name_prefix$i &
	}

	for job in `jobs -p`
	do
		wait $job
		res=$?
		[[ $res -eq "1" ]] && let "failures += 1"
	done
	[[ $failures -ne "0" ]] && echo $failures failed, see result for detail && exit

	echo === Test all pass at round $j and cleaning up...
	# cleanup by removing pods
	for (( i = 0; $i < $parallel; i++ )) {
		kubectl delete -f yamls/$pod_name_prefix$i.yaml &> /dev/null
	}
}
