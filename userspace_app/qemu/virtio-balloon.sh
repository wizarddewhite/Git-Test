# free page reporting
sudo ~/git/qemu/x86_64-softmmu/qemu-system-x86_64 -m 8G,slots=256,maxmem=128G \
    -machine pc,nvdimm -cpu host,pse=on \
    -smp 8,maxcpus=16 --enable-kvm \
    -drive file=~/guest/fedora31.img,format=raw -nographic \
    -drive file=~/guest/project.img,format=raw \
    --device virtio-balloon,free-page-reporting=true

# free page hinting
-object iothread,id=iothread1 --device virtio-balloon,free-page-hint=true,iothread=iothread1
