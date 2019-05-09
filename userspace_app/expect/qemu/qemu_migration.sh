#!/usr/bin/expect
# expect -f qemu_monitor.sh localhost 55555

#If it all goes pear shaped the script will timeout after 20 seconds.
set timeout 20
set index 0

set server [lindex $argv 0]
set port [lindex $argv 1]

set prot [lindex $argv 2]
set dest [lindex $argv 3]
set dest_port [lindex $argv 4]

#This spawns the ssh program and connects it to the variable server
spawn telnet $server $port
#The script expects
expect -nocase "qemu"

send "info status\r"

expect {
	"paused" {
		send "info status\r"
		incr index
		sleep 5
		if {$index < 10} {
			exp_continue
		}
	}
	"running"
}

if {$index == 10} {
	send_user "failed\n"
	exit
}

send_user "start migration $prot:$dest:$dest_port\n"

send "migrate -d $prot:$dest:$dest_port\r"
sleep 5
send "info migrate\r"

expect {
	"Migration status: active" {
		send "info migrate\r"
		sleep 5
		exp_continue
	}
	"Migration status: completed"
}

send_user "migration done\n"
