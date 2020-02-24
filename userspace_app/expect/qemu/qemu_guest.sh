#!/usr/bin/expect
# expect -f qemu_guest.sh command_file user passwd

#If it all goes pear shaped the script will timeout after 20 seconds.
set timeout 600
set index 0

set cmd_file [lindex $argv 0]
set user [lindex $argv 1]
set passwd [lindex $argv 2]
set fd [open $cmd_file r]
set qemu_cmd [read $fd]
close $fd

send_user "$qemu_cmd\n"

spawn bash -c $qemu_cmd

expect {
	timeout {
		send_user "bootup timeout\n"
		exit
	}
	"login:"
}
send "$user\r"
expect "Password:"
send -- "$passwd\r"

## kernel build
#send "cd git/linux\r"
#send "make clean && make -j4\r"

## sysbench
#send "./sysbench.sh\r"

## netperf
#send "./netperf.sh\r"

interact
