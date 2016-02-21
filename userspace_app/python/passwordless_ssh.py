#!/usr/bin/python
from optparse import OptionParser
import sys ,string ,platform ,types ,os ,getpass
import pexpect, socket, time, signal 

def ssh_cmd(host, passwd, cmd):
	ssh_newkey = 'Are you sure you want to continue connecting'
	conditions = ['Connection timed out', 'No route to host', ssh_newkey, 'password:', pexpect.EOF, pexpect.TIMEOUT]

	if debug:
		print ""
		print cmd

	# connecting
	child = pexpect.spawn(cmd)
	ret = child.expect(conditions)
	if debug:
		child.logfile = sys.stdout
	if ret == 0:
		# ssh server down?
		return -1
	if ret == 1:
		# no network connection
		return -2
	if ret == 2:
		child.sendline('yes')
		ret = child.expect(conditions)
	if ret == 3:
		child.sendline(passwd)
		ret = child.expect(['Permission denied', pexpect.TIMEOUT, pexpect.EOF], timeout = 350)
	if ret == 0:
		# wrong password
		child.kill(signal.SIGINT)
		return -3

	r = child.before
	child.kill(signal.SIGINT)
	return r

# we should be root
#user = getpass.getuser()
#if user != "root":
#	print "this script need to be run as root"
#	exit()

# pass the option
usage = "usage: %prog -f machine_list_file"
parser = OptionParser(usage=usage)
parser.add_option("-f", "--file", dest="filename",
		help="machine list file, format is: ip user passwd", metavar="FILE")
parser.add_option("-v", "--verbos", action="store_true", dest="verbose", default=False,
		help="list the machines")
parser.add_option("-p", "--preserve", action="store_true", dest="preserve", default=False,
		help="preserve old pub keys")

(options, args) = parser.parse_args()
if options.filename == None:
	print "ERROR! "
	print "     please provide the machine list file"
	print "     %s -h to show the usage" %sys.argv[0]
	exit() 

debug = options.verbose

preserve = options.preserve

# read in the machine list file
f = open(options.filename, "r")
machines  = {}
duplicate = []
failed    = {}
for line in f:
	strip_line = line.strip()
	machine = strip_line.split()
	if machine[0] in machines:
		duplicate.append(machine)
	machines[machine[0]] = machine[1:]

if debug:
	print "machine lists:"
	for machine in machines:
		print "  %s %s" %(machine, machines[machine])

# check whether there is duplicated machines
if len(duplicate) != 0:
	print "oops, you entered the same machine more than once"
	for machine in duplicate:
		print "  %s" %machine

	ans = raw_input("you want to continue?(y/n)")
	if ans == "y":
		print "go on"
	elif ans == "n":
		print "exit"
		exit()
	else:
		print "not correct, just exit"
		exit()

# ssh on each host, generate public key 
# gather all their public key
pub_tmp_file = "public_key_tmp_file"
cmd = 'rm -f %s' %pub_tmp_file
os.system(cmd)
pubkey_file = open(pub_tmp_file, "w")

for machine in machines:
	cmd = 'ssh -o ConnectTimeout=100 root@%s "rm -f /root/.ssh/id_rsa*; ssh-keygen -t rsa -f /root/.ssh/id_rsa -N \'\' -q; cat /root/.ssh/id_rsa.pub"' %(machine)
	ret = ssh_cmd(machine, machines[machine][0], cmd)

	if ret == -1:
		failed[machine] = "ssh server is not started? or the firewall block ssh?"
		continue
	elif ret == -2:
		failed[machine] = "no network path to host"
		continue
	elif ret == -3:
		failed[machine] = "password is not correct"
		continue
	
	print "get pub key from %s" %machine
	if ret != None:
		pubkey_file.write(ret)

	if not(preserve):
		cmd = 'ssh -o ConnectTimeout=100 root@%s "rm -f /root/.ssh/authorized_keys2"' %machine
		ret = ssh_cmd(machine, machines[machine][0], cmd)

pubkey_file.close()

if debug:
	cmd = "cat %s" %pub_tmp_file
	os.system(cmd)

# copy the publick key to every host
for machine in machines:
	if machine in failed:
		continue

	print "copy pub key to %s" %machine
	cmd = 'scp -o ConnectTimeout=100  %s root@%s:%s' %(pub_tmp_file, machine, pub_tmp_file)
	ret = ssh_cmd(machine, machines[machine][0], cmd)

	cmd = 'ssh -o ConnectTimeout=100 root@%s "cat %s >> /root/.ssh/authorized_keys2"' %(machine, pub_tmp_file)
	ret = ssh_cmd(machine, machines[machine][0], cmd)

# show the failed list
if len(failed):
	print ""
	print "Failed machine list:"
	print "Address:              Reason:"
	print "----------------------------------------"
	for machine in failed:
		print "%s,       %s" %(machine, failed[machine])
exit()
