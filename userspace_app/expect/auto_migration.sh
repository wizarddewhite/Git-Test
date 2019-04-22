#!/usr/bin/expect
# expect -f qemu_guest.sh command_file

set timeout 200
set index 0

set cmd_file [lindex $argv 0]
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

expect -i $source "login:"

#interact -i $source

spawn telnet localhost 55555
set telnet $spawn_id
#The script expects
expect -i $telnet -nocase "qemu"

send_user "start migration \n"

send -i $telnet "migrate -d tcp:0:4444\r"
sleep 10
send -i $telnet "info migrate\r"

expect {
	-i $telnet "Migration status: active" {
		send "info migrate\r"
		sleep 10
		exp_continue
	}
	-i $telnet "Migration status: completed"
}

send_user "migration done\n"
exit 0
