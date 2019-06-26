#!/usr/bin/expect
# expect -f qemu_guest.sh command_file user passwd

set timeout 200
set index 0

set cmd_file [lindex $argv 0]
set user [lindex $argv 1]
set passwd [lindex $argv 2]
set fd [open $cmd_file r]
set tmp [read $fd]
set tmp [string trimright $tmp \n]
close $fd

#set mon_addr [lindex $argv 1]
#set mon_port [lindex $argv 2]
#
#set mig_prot [lindex $argv 3]
#set mig_addr [lindex $argv 4]
#set mig_port [lindex $argv 5]

set source_cmd [format %s%s ${tmp} " -monitor telnet:127.0.0.1:55555,server,nowait"]
send_user "$source_cmd\n"
spawn bash -c $source_cmd
set source $spawn_id

set dest_cmd [format %s%s%s ${tmp} " -monitor telnet:127.0.0.1:55556,server,nowait" " -incoming tcp:0:4444"]
send_user "$dest_cmd\n"
spawn bash -c $dest_cmd 
set dest $spawn_id

expect {
	-i $source timeout {
		send_user "\nError: bootup timeout\n"
		exit -1
	}
	-i $source "login:" {
		send -i $source "$user\r"
		expect -i $source "Password:"
		send -i $source "$passwd\r"
	}
}

#interact -i $source
#send -i $source "cd git/linux\r"
#send -i $source "make clean && make -j4 bzImage\r"

spawn telnet localhost 55555
set source_telnet $spawn_id
spawn telnet localhost 55556
set dest_telnet $spawn_id

#The script expects
expect -i $source_telnet -nocase "qemu"

send_user "start migration \n"

# setup capability for both side
send -i $source_telnet "migrate_set_capability multifd on\r"
send -i $dest_telnet "migrate_set_capability multifd on\r"
sleep 5

send -i $source_telnet "migrate -d tcp:0:4444\r"
sleep 5
send -i $source_telnet "info migrate\r"

# check dirty sync count
expect {
	-i $source_telnet -indices -re {dirty sync count: *([0-9]{1,4})} {
		set sync_count $expect_out(1,string)
		puts -nonewline "\n### current dirty sync count :"
		puts $sync_count
		sleep 3
		send -i $source_telnet "info migrate\r"
		if {$sync_count <= 1} {
			exp_continue
		}
	}
	-i $source_telnet eof {
		send_user "\nsource vm terminated unexpectedly!\n"
		exit -1
	}
}

expect {
	-i $source_telnet "Migration status: active" {
		send -i $source_telnet "info migrate\r"
		sleep 10
		exp_continue
	}
	-i $source_telnet "Migration status: completed" {}
	-i $source_telnet eof {
		send_user "\nsource vm terminated unexpectedly!\n"
		exit -1
	}
}

send_user "migration done on source\n"
#interact -i $dest

#send -i $dest "ls /etc/hosts\r"
send -i $dest "shutdown now\r"
expect {
	-i $dest eof {
		send_user "\ndestination power off successfully!\n"
	}
	-i $dest timeout {
		send_user "\nError: destination no response!\n"
		exit -1
	}
}

exit 0
