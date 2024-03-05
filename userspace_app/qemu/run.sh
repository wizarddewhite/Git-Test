sudo ~/git/qemu/build/qemu-system-x86_64 -m 4G,slots=32,maxmem=32G \
        -smp 8 --enable-kvm \
        -drive file=/home/richard/guest/fedora.img,format=raw \
        -drive file=/home/richard/guest/project.img,format=qcow2 \
        -nographic

sudo ~/git/qemu/build/qemu-system-x86_64 -m 6G,slots=32,maxmem=32G \
        -smp 8 --enable-kvm \
        -drive file=/home/richard/guest/fedora.img,format=raw \
        -drive file=/home/richard/guest/project.img,format=qcow2 \
        -nographic \
        -object memory-backend-ram,id=mem1,size=3G -object memory-backend-ram,id=mem2,size=3G \
        -numa node,nodeid=0,memdev=mem1 -numa node,nodeid=1,memdev=mem2

# with vnc
sudo ~/git/qemu/build/qemu-system-x86_64 -m 4G,slots=32,maxmem=32G \
        -smp 8 --enable-kvm \
        -drive file=/home/richard/guest/fedora.img,format=raw \
        -drive file=/home/richard/guest/project.img,format=qcow2 \
        -vnc :0

# start raw kernel with customized initrd
# where rootfs is built from hello.c. Read hello.c for usage
# in qemu monitor, use "change vnc password"
sudo /home/richard/git/qemu/build/qemu-system-x86_64  \
  -m 4G -smp 8 --enable-kvm \
  -kernel /home/richard/git/linux/arch/x86/boot/bzImage \
  -initrd ./rootfs -append "root=/dev/ram rdinit=/hello" \
  -vnc :0,password -monitor stdio
