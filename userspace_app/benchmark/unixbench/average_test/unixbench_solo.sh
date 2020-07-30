#!/usr/bin/expect

set argsCount [llength $argv]

if {$argsCount < 2} {
    puts $argv
    puts "Usage: unixbench_solo.sh index result_dir \[runtime\]"
    exit 1
}

#If it all goes pear shaped the script will timeout after 20 seconds.
set timeout 10
#First argument is assigned to the variable name
set idx [lindex $argv 0]
set result_dir [lindex $argv 1]
set runtime "runc" 

if {$argsCount == 3} {
	set runtime [lindex $argv 2]
}

# start redis
spawn pouch run -m 2g --annotation io.alibaba.pouch.vm.passthru.cpus=2 --runtime=$runtime --name container-unixbench$idx -v $result_dir:/unixbench_result/ -d tutum/unixbench

expect {
	"Error: " {
		send_user "Failed to start pod\n"
		exit -1
	}
	timeout {
		send_user "pod started\n"
	}
}

sleep 1

spawn pouch exec -it container-unixbench$idx sh
expect {
	"#" {  }
	timeout {
		send_user "Unable to login \n"
        exit -1
	}
}

# run unixbench
send "rm -f results/*\r"
sleep 3
send "./Run pipe \r"

set timeout 10000
expect {
	"System Benchmarks Index Score" {
		send_user "Done now\n"
	}
	timeout {
		send_user "!!! Test failed\n"
	}
}

sleep 5
send "rm -f results/*.html\r"
send "rm -f results/*.log\r"
send "cp results/* /unixbench_result/\r"

# terminate
spawn pouch rm -f container-unixbench$idx

set timeout 120
expect {
	"Error: " {
		send_user "Failed to stop pod\n"
		exit -1
	}
	timeout {
		send_user "Not managed to terminate pod\n"
	}
	"#" {
        	exit 0
	}
}
