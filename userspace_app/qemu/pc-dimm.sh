# use hugepage as pc-dimm
# https://github.com/qemu/qemu/blob/master/docs/memory-hotplug.txt

# enable hugepage and mount to file
echo 4 > /sys/devices/system/node/node1/hugepages/hugepages-1048576kB/nr_hugepages
mkdir /dev/hugepages1G
mount -t hugetlbfs -o pagesize=1G none /mnt/hugetlbfs

# use hugepage in qemu command line

sudo ~/git/qemu/x86_64-softmmu/qemu-system-x86_64 \
	-smp 4,maxcpus=16 --enable-kvm \
	-drive file=/home/richard/guest/fedora29.img,format=raw -nographic \
	-drive file=/home/richard/guest/project.img,format=raw \
	-m 1G,slots=4,maxmem=128G \
	-object memory-backend-file,id=mem1,mem-path=/mnt/hugetlbfs,size=4G \
	-device pc-dimm,id=dimm1,memdev=mem1

