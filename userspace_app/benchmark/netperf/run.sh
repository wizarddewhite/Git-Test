# server
netserver &

# client

# RR for latency, STREAM for qps
netperf -H server_ip -t tcp_rr -l 60

netperf -H $SERVER_IP -t TCP_RR -l 120 -- -l 1,1
netperf -H $SERVER_IP -t TCP_RR -l 120 -- -l 32,1024
netperf -H $SERVER_IP -t TCP_STREAM -l 120 -- -l 1,1
netperf -H $SERVER_IP -t TCP_STREAM -l 120 -- -l 32,1024

netperf -H $SERVER_IP -t UDP_RR -l 120 -- -l 1,1
netperf -H $SERVER_IP -t UDP_RR -l 120 -- -l 32,1024
netperf -H $SERVER_IP -t UDP_STREAM -l 120 -- -l 1,1
netperf -H $SERVER_IP -t UDP_STREAM -l 120 -- -l 32,1024
