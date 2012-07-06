#!/usr/bin/python
import os
import sys
import pexpect
import base64


def main():
	
	host_name = raw_input("Enter the host name: ")
	
	if host_name == "haven":
		host_name = "haven.au.ibm.com"
	elif host_name == "ford11":
		host_name = "ford11.ltc.austin.ibm.com"
	else:
		print "Unknown host name"
		exit()
	
	telnet_cmd = "telnet %s" %(host_name)
	print "%s" %telnet_cmd
	
	try:
		child = pexpect.spawn(telnet_cmd)
		child.logfile = sys.stdout
		i = child.expect(['Username: ', 'Unable to connect to remote host'], 30)
		if i == 0:
			child.sendline('ywywyang@cn.ibm.com')
			child.expect('Password:')
			child.sendline('0502shyw')
			i == child.expect(['BSO Authentication Successful', 'BSO Authentication Error'])
			if i == 0:
				print "Passed"
			else:
				print "Try Again"
		else:
			print "You passed the BSO already"
			child.close()
	except pexpect.EOF:
		child.close()
	except pexpect.TIMEOUT:
		print "Connection timeout"
		child.close()
	else:
		child.close()
	
if __name__ == "__main__":
	main()
