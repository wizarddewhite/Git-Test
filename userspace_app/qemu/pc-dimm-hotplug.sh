# start with memslots
sudo ~/git/qemu/x86_64-softmmu/qemu-system-x86_64 -m 4G,slots=4,maxmem=128G \
	-smp 4,maxcpus=16 --enable-kvm \
	-drive file=/home/richard/guest/fedora29.img,format=raw -nographic

# execute in monitor
object_add memory-backend-ram,id=ram0,size=1G
device_add pc-dimm,id=dimm0,memdev=ram0

# hotplug with node info
sudo ~/git/qemu/x86_64-softmmu/qemu-system-x86_64 -m 4G,slots=4,maxmem=128G \
	-smp 4,maxcpus=16 --enable-kvm \
	-drive file=/home/richard/guest/fedora29.img,format=raw -nographic
        -numa node,nodeid=0 -numa node,nodeid=1 -numa node,nodeid=2 -numa node,nodeid=3

# execute in monitor
object_add memory-backend-ram,id=ram0,size=1G
device_add pc-dimm,id=dimm0,memdev=ram0,node=0
