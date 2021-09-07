1. Install

For centos:

# yum update -y
# yum install iperf -y

2. Run Test

Server:
# iperf -s -f K

Client:
# iperf -c server_ip -i 1 -t 600
