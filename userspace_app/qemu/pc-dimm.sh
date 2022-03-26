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

# specify numa

-numa node,nodeid=0 -numa node,nodeid=1

# or new way

-object memory-backend-ram,id=mem1,size=2G -object memory-backend-ram,id=mem2,size=2G \
-numa node,nodeid=0,memdev=mem1 -numa node,nodeid=1,memdev=mem2

# set numa distance, a vm with 8 NUMA and following distance patten
# {10, 12, 12, 12, 32, 32, 32, 32},
# {12, 10, 12, 12, 32, 32, 32, 32},
# {12, 12, 10, 12, 32, 32, 32, 32},
# {12, 12, 12, 10, 32, 32, 32, 32},
# {32, 32, 32, 32, 10, 12, 12, 12},
# {32, 32, 32, 32, 12, 10, 12, 12},
# {32, 32, 32, 32, 12, 12, 10, 12},
# {32, 32, 32, 32, 12, 12, 12, 10},

sudo ~/git/qemu/build/qemu-system-x86_64 -m 8G,slots=32,maxmem=32G \
        -smp 8 --enable-kvm \
        -drive file=/home/richard/guest/fedora.img,format=raw \
        -drive file=/home/richard/guest/project.img,format=qcow2 \
        -nographic \
        -object memory-backend-ram,id=mem1,size=1G -object memory-backend-ram,id=mem2,size=1G \
        -object memory-backend-ram,id=mem3,size=1G -object memory-backend-ram,id=mem4,size=1G \
        -object memory-backend-ram,id=mem5,size=1G -object memory-backend-ram,id=mem6,size=1G \
        -object memory-backend-ram,id=mem7,size=1G -object memory-backend-ram,id=mem8,size=1G \
        -numa node,nodeid=0,memdev=mem1 -numa node,nodeid=1,memdev=mem2 \
        -numa node,nodeid=2,memdev=mem3 -numa node,nodeid=3,memdev=mem4 \
        -numa node,nodeid=4,memdev=mem5 -numa node,nodeid=5,memdev=mem6 \
        -numa node,nodeid=6,memdev=mem7 -numa node,nodeid=7,memdev=mem8 \
        -numa dist,src=0,dst=0,val=10 -numa dist,src=0,dst=1,val=12 -numa dist,src=0,dst=2,val=12 -numa dist,src=0,dst=3,val=12 \
        -numa dist,src=0,dst=4,val=32 -numa dist,src=0,dst=5,val=32 -numa dist,src=0,dst=6,val=32 -numa dist,src=0,dst=7,val=32 \
        -numa dist,src=1,dst=1,val=10 -numa dist,src=1,dst=2,val=12 -numa dist,src=1,dst=3,val=12 \
        -numa dist,src=1,dst=4,val=32 -numa dist,src=1,dst=5,val=32 -numa dist,src=1,dst=6,val=32 -numa dist,src=1,dst=7,val=32 \
        -numa dist,src=2,dst=2,val=10 -numa dist,src=2,dst=3,val=12 \
        -numa dist,src=2,dst=4,val=32 -numa dist,src=2,dst=5,val=32 -numa dist,src=2,dst=6,val=32 -numa dist,src=2,dst=7,val=32 \
        -numa dist,src=3,dst=3,val=10 \
        -numa dist,src=3,dst=4,val=32 -numa dist,src=3,dst=5,val=32 -numa dist,src=3,dst=6,val=32 -numa dist,src=3,dst=7,val=32 \
        -numa dist,src=4,dst=4,val=10 -numa dist,src=4,dst=5,val=12 -numa dist,src=4,dst=6,val=12 -numa dist,src=4,dst=7,val=12 \
        -numa dist,src=5,dst=5,val=10 -numa dist,src=5,dst=6,val=12 -numa dist,src=5,dst=7,val=12 \
        -numa dist,src=6,dst=6,val=10 -numa dist,src=6,dst=7,val=12 \
        -numa dist,src=7,dst=7,val=10
