Qemu command example:

/home/richard/git/qemu/x86_64-softmmu/qemu-system-x86_64 -machine pc,nvdimm \
	-m 4G,slots=4,maxmem=128G -smp 8 --enable-kvm \
	-drive file=/home/richard/guest/fedora29.img,format=raw -nographic \
	-drive file=/home/richard/guest/project.img,format=raw \
	-object memory-backend-file,id=mem1,share=on,mem-path=/dev/dax0.0,size=1G,align=2M \
	-device nvdimm,id=nvdimm1,memdev=mem1,label-size=128k \
	-object memory-backend-file,id=mem2,share=on,mem-path=/dev/dax0.1,size=2G,align=2M \
	-device nvdimm,id=nvdimm2,memdev=mem2,label-size=128k

Or use memory as backend:

    -object memory-backend-ram,id=mem0,size=8G \
    -device nvdimm,id=vm0,memdev=mem0,addr=0x144000000,label-size=128k

Usually not necessary to specify the start address.

In case backend is a pmem, we should configure like this:

# lsblk
NAME    MAJ:MIN RM   SIZE RO TYPE MOUNTPOINT
pmem0   259:0    0  49.2G  0 disk

# parted /dev/pmem0 mklabel gpt
# parted -a opt /dev/pmem0 mkpart primary ext4 0% 100%
# lsblk
pmem0     259:0    0  49.2G  0 disk
--pmem0p1 259:3    0  49.2G  0 part
# mkfs.ext4 -L pmem1 /dev/pmem0p1
# lsblk --fs
pmem0
--pmem0p1 ext4   pmem1 03d9f92d-0252-4163-bc5f-a9a5a78d62db

# mount -o dax /dev/pmem0p1 /mnt
# dd if=/dev/zero of=/mnt/backend1 bs=2G count=1

-object memory-backend-file,id=mem1,share=on,mem-path=/mnt/backend1,size=2G,align=2M
