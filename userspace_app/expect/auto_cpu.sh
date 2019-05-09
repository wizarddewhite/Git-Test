#!/usr/bin/expect
# expect -f auto_cpu qemu_command user passwd

set timeout 200
set index 0

set cmd_file [lindex $argv 0]
set user [lindex $argv 1]
set passwd [lindex $argv 2]
set fd [open $cmd_file r]
set cmd [read $fd]
set cmd [string trimright $cmd \n]
close $fd

set cmd [format %s%s ${cmd} " -monitor telnet:127.0.0.1:55555,server,nowait"]

puts $cmd

spawn bash -c $cmd
set guest $spawn_id

expect {
	-i $guest timeout {
		send_user "\nError: bootup timeout\n"
		exit -1
	}
	-i $guest "login:" {
		send -i $guest "$user\r"
		expect -i $guest "Password:"
		send -i $guest "$passwd\r"
	}
}

send -i $guest "lscpu | head\r"
expect -i $guest -indices -re {CPU\(s\): *([0-9]{1,4})}
set cpus $expect_out(1,string)
puts -nonewline "\n### Original online CPUs:"
puts $cpus
incr $cpus

expect -i $guest "\[$user"
sleep 1

spawn telnet localhost 55555
set telnet $spawn_id
expect {
	-i $telnet -nocase "qemu" {
		send -i $telnet "device_add qemu64-x86_64-cpu,id=cpu1,apic-id=$cpus \r"
	}
}

sleep 1
send -i $guest "lscpu | head\r"
expect -i $guest -indices -re {CPU\(s\): *([0-9]{1,4})}
set new_cpus $expect_out(1,string)
puts -nonewline "\n### Current onlined CPUs:"
puts $new_cpus
send -i $guest "shutdown now\r"

expect {
	-i $guest eof {
		send_user "\ndestination power off successfully!\n"
	}
	-i $guest timeout {
		send_user "\nError: destination no response!\n"
		exit -1
	}
}

exit 0
