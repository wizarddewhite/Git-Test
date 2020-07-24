#!/usr/bin/expect

set argsCount [llength $argv]

if {$argsCount < 2} {
    puts $argv
    puts "Usage: redis_solo.sh index result_dir \[runtime\]"
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
spawn pouch run --runtime=$runtime --name container-redis$idx -v $result_dir:/redis_result/ -d redis

expect {
	"Error: " {
		send_user "Failed to start pod\n"
		exit -1
	}
	timeout {
		send_user "pod started\n"
	}
}

spawn pouch exec -it container-redis$idx sh
expect {
	"#" {  }
	timeout {
		send_user "Unable to login \n"
        exit -1
	}
}

# run the benchmark
send "redis-benchmark -h 127.0.0.1 -p 6379 -t set -d 3 -r 10000000 -n 10000000 -c 100 -q > /redis_result/container-redis$idx.result\r"

set timeout 10000
expect {
	"#" {
		send_user "Test finished\n"
	}
	timeout {
		send_user "Test failed\n"
	}
}

# terminate
spawn pouch rm -f container-redis$idx

set timeout 10
expect {
	"Error: " {
		send_user "Failed to stop pod\n"
		exit -1
	}
	"#" {
        exit 0
	}
}
