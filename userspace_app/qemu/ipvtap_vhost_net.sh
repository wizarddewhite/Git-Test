#  Topology
#
#  +---------------------------------------+
#  |                                       |
#  |  +----------------------------+       |
#  |  |        NS: ns0             |       |
#  |  |   +--------------------+   |       |
#  |  |   |    Guest           |   |       |
#  |  |   |                    |   |       |
#  |  |   |  ens3              |   |       |
#  |  |   | 10.1.1.1           |   |       |
#  |  |   | 1a:46:0b:ca:bc:7b  |   |       |
#  |  |   |                    |   |       |
#  |  |   +--------------------+   |       |
#  |  |                            |       |
#  |  |    ipvt0                   |       |
#  |  |    10.1.1.1                |       |
#  |  |    1a:46:0b:ca:bc:7b       |       |
#  |  +----------------------------+       |
#  |                |                      |
#  |                |                      |
#  |               enp0s3                  |
#  |              10.2.1.1                 |
#  |              1a:46:0b:ca:bc:7b        |
#  +---------------------------------------+
#
# ip link add link eth0 name ipvt0 type ipvtap
# ip netns add ns0
# ip link set ipvt0 netns ns0
# ip netns exec ns0 ip link set lo up
# ip netns exec ns0 ip link set ipvt0 up
# ip netns exec ns0 ip addr add 10.1.1.1/26 dev ipvt0
# ip netns exec ns0 ip route add default dev ipvt0

/home/dev/paoxiao/qemu/build/qemu-system-x86_64 -m 8G -smp 8 \
	--enable-kvm -nographic -trace events=/tmp/events \
	-drive file=/home/dev/paoxiao/guest/fedora.img,format=raw \
	-netdev tap,fd=31,id=hostnet0,vhost=on,vhostfd=40 \
	    31<>/dev/tap`cat /sys/class/net/ipvt0/ifindex` \
	    40<>/dev/vhost-net \
	-device virtio-net-pci,netdev=hostnet0,id=net0,mac=1a:46:0b:ca:bc:7b

# after bootup, we need the following config in guest
#
# Version 1, works but not correct:
#
# ip addr add 10.1.1.1/26 dev ens3
# ip route add 10.2.1.1 dev ens3
# ip route add default via 10.2.1.1 dev ens3
# arp -i ens3 -s 10.2.1.1 1a:46:0b:ca:bc:7b
#
# Version 2
#
# ip addr add 10.1.1.1/26 dev ens3
# ip route add default dev ens3
# ip link set ens3 arp off
