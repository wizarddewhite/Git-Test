sudo ~/git/qemu/x86_64-softmmu/qemu-system-x86_64 -m 4G,slots=4,maxmem=128G \
	-smp 4,maxcpus=16 --enable-kvm \
	-drive file=/home/richard/guest/fedora29.img,format=raw -nographic


# in monitor
# deprecated
cpu-add 4
# new fashion
device_add qemu64-x86_64-cpu,id=cpu1,apic-id=5

device_add qemu64-x86_64-cpu,id=cpu1,socket-id=2,core-id=1,thread-id=1
