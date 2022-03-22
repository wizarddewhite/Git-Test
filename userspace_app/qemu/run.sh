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
