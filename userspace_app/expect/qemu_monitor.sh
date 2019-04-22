#!/usr/bin/expect
# expect -f qemu_monitor.sh localhost 55555

#If it all goes pear shaped the script will timeout after 20 seconds.
set timeout 20
set index 0
#First argument is assigned to the variable server
set server [lindex $argv 0]
#Second argument is assigned to the variable port
set port [lindex $argv 1]
#This spawns the ssh program and connects it to the variable server
spawn telnet $server $port
#The script expects
expect -nocase "qemu"

send "info status\r"

expect {
	"paused" {
		send "info status\r"
		incr index
		sleep 1
		if {$index < 10} {
			exp_continue
		}
	}
	"running"
}

if {$index == 10} {
	send_user "failed\n"
} else {
	send_user "done\n"
}
