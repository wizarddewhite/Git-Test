iptables -N INET_IN
iptables -A INPUT -j INET_IN
iptables -N user
iptables -A INET_IN -j user
iptables -A user -p tcp --dport 34564
iptables -L user -Z
