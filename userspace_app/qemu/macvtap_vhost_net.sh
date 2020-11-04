#  Topology
#
#  +---------------------------------------------------------------------+
#  |                                                                     |
#  |           NS: tap0                         NS: tap1                 |
#  |  +----------------------------+          +-------------------+      |
#  |  |   +--------------------+   |          |                   |      |
#  |  |   |                    |   |          |                   |      |
#  |  |   |                    |   |          |                   |      |
#  |  |   | eth0               |   |          |                   |      |
#  |  |   | 10.1.1.1           |   |          |                   |      |
#  |  |   | 1a:46:0b:ca:bc:7b  |   |          |                   |      |
#  |  |   +--------------------+   |          |                   |      |
#  |  |                            |          |                   |      |
#  |  |    macvtap0                |          |   macvtap1        |      |
#  |  |    1a:46:0b:ca:bc:7b       |          |   10.1.1.2        |      |
#  |  |                            |          |                   |      |
#  |  +----------------------------+          +-------------------+      |
#  |                            \              /                         |
#  |                                 \     /                             |
#  |                                enp0s3                               |
#  |                                                                     |
#  +---------------------------------------------------------------------+
#
# ip link add link eth0 name macvtap0 type macvtap mode bridge
# ip link set macvtap0 address 1a:46:0b:ca:bc:7b up
# cat /sys/class/net/macvtap0/ifindex , output will be used in cmd
# for other setup, refer to sys_admin/network/macvtap.txt.
# remember to remove ip on macvtap0 and up it.

/home/dev/paoxiao/qemu/build/qemu-system-x86_64 -m 8G -smp 8 \
	--enable-kvm -nographic -trace events=/tmp/events \
	-drive file=/home/dev/paoxiao/guest/fedora.img,format=raw \
	-netdev tap,fd=31,id=hostnet0,vhost=on,vhostfd=40 \
	    31<>/dev/tap`cat /sys/class/net/macvtap0/ifindex` \
	    40<>/dev/vhost-net \
	-device virtio-net-pci,netdev=hostnet0,id=net0,mac=1a:46:0b:ca:bc:7b

# In guest, do the following config, then we can ssh 10.1.1.1 from NS:tap1
# ip addr add 10.1.1.1/24 dev eth0


#  Topology, two guests in its own namespace with macvtap
#
#  +---------------------------------------------------------------------------+
#  |                                                                           |
#  |           NS: tap0                                NS: tap1                |
#  |  +----------------------------+          +----------------------------+   |
#  |  |   +--------------------+   |          |   +--------------------+   |   |
#  |  |   |                    |   |          |   |                    |   |   |
#  |  |   |                    |   |          |   |                    |   |   |
#  |  |   | eth0               |   |          |   | eth0               |   |   |
#  |  |   | 10.1.1.1           |   |          |   | 10.1.1.2           |   |   |
#  |  |   | 1a:46:0b:ca:bc:7b  |   |          |   | 1a:46:0b:ca:bc:7c  |   |   |
#  |  |   +--------------------+   |          |   +--------------------+   |   |
#  |  |                            |          |                            |   |
#  |  |    macvtap0                |          |    macvtap0                |   |
#  |  |    1a:46:0b:ca:bc:7b       |          |    1a:46:0b:ca:bc:7c       |   |
#  |  |                            |          |                            |   |
#  |  +----------------------------+          +----------------------------+   |
#  |                            \              /                               |
#  |                                 \     /                                   |
#  |                                enp0s3                                     |
#  |                                                                           |
#  +---------------------------------------------------------------------------+
