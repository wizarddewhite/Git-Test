# bridge network

## config bridge on host

#RHEL, physical device behind bridge
# /etc/sysconfig/network-scripts/ifcfg-br0
DEVICE=br0
TYPE=Bridge
ONBOOT=yes
BOOTPROTO=dhcp
NM_CONTROLLED=no
DEFROUTE=yes
STP=no

# /etc/sysconfig/network-scripts/ifcfg-enp4s0f0
NAME="enp4s0f0"
DEVICE="enp4s0f0"
ONBOOT=yes
BOOTPROTO=none
UUID="a749c661-ede7-4cb7-9450-2491782e1e8b"
HWADDR=00:1e:67:a0:14:6b
IPV6INIT=yes
TYPE=Ethernet
BRIDGE=br0
NM_CONTROLLED=no
IPADDR=0.0.0.0

## or create bridge manually
sudo brctl addbr br0
sudo ifconfig br0 192.168.122.1 up

## create tap interface
sudo ip tuntap add tap0 mode tap
sudo ifconfig tap0 0.0.0.0 up
sudo brctl addif br0 tap0

# specify the device type

## rtl8139
-netdev tap,id=tapnet,ifname=tap0,script=no -device rtl8139,netdev=tapnet

## virtio-net
-netdev tap,id=tapnet,ifname=tap0,script=no -device virtio-net-pci,netdev=tapnet

# configure guest ip same as bridge subnet
ifconfig ens2 192.168.122.100 up
route add default gw 192.168.122.1
