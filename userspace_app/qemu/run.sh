sudo ~/git/qemu/build/qemu-system-x86_64 -m 4G,slots=32,maxmem=32G \
        -smp 8 --enable-kvm \
        -drive file=/home/richard/guest/fedora.img,format=raw \
        -drive file=/home/richard/guest/project.img,format=qcow2 \
        -nographic

