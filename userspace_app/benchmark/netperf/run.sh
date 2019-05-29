# server
netserver &

# client
netperf -H server_ip -t tcp_rr -l 60
