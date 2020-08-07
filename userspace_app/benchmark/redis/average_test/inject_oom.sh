#!/usr/bin/expect

set argsCount [llength $argv]

if {$argsCount < 1} {
    puts $argv
    puts "Usage: inject_oom.sh oom_dir \[runtime\]"
    exit 1
}

set oom_dir [lindex $argv 0]
set runtime "runc"

if {$argsCount == 2} {
	set runtime [lindex $argv 1]
}

#If it all goes pear shaped the script will timeout after 20 seconds.
set timeout 10
#First argument is assigned to the variable name

# start redis
spawn pouch run -m 16g --annotation io.alibaba.pouch.vm.passthru.cpus=2 --annotation io.alibaba.pouch.vm.passthru.memory="16Gi" \
	--runtime=$runtime --name oom-inject -v $oom_dir:/test/ -it reg.docker.alibaba-inc.com/alibase/alios7u2 sh

expect {
	"#" {  }
	timeout {
		send_user "Unable to login \n"
        exit -1
	}
}

# trigger oom
send "/test/oom.sh /test/\r"

set iter 1
set timeout 10000
expect {
	"#" {
		if {$iter < 3} {
			incr iter
			send "/test/oom.sh /test/\r"
			exp_continue
		}
		send_user "Test finished\n"
	}
	timeout {
		send_user "!!! Test failed\n"
	}
}

# terminate
spawn pouch rm -f oom-inject

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
