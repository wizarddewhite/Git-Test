#!/usr/bin/expect

#If it all goes pear shaped the script will timeout after 20 seconds.
set timeout 20
#First argument is assigned to the variable name
set server [lindex $argv 0]
#Second argument is assigned to the variable user
set user [lindex $argv 1]
#Third argument is assigned to the variable password
set password [lindex $argv 2]
#This spawns the ssh program and connects it to the variable server
spawn ssh $user@$server
#The script expects Password
expect "*?assword:"
#The script sends the password variable
send -- "$password\r"
#This hands control of the keyboard over two you (Nice expect feature!)
interact
