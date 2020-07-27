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
spawn pouch run -m 2g --runtime=$runtime --name oom-inject -v $oom_dir:/test/ -it redis sh

expect {
	"#" {  }
	timeout {
		send_user "Unable to login \n"
        exit -1
	}
}

# run a.out to trigger oom
send "/test/a.out\r"

set timeout 10000
expect {
	"#" {
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
