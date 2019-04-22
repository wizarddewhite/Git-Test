#!/usr/bin/expect
# expect -f qemu_guest.sh command_file

#If it all goes pear shaped the script will timeout after 20 seconds.
set timeout 20
set index 0

set cmd_file [lindex $argv 0]
set fd [open $cmd_file r]
set qemu_cmd [read $fd]
close $fd

send_user "$qemu_cmd\n"

spawn bash -c $qemu_cmd

expect "login:"

interact
