# command line, not only device/drive but also add bus
    -device nec-usb-xhci,id=xhci                                    \
    -device usb-uas,id=uas,bus=xhci.0                               \
    -drive if=none,id=disk1,file=~/guest/swap.img,format=raw \
    -device scsi-hd,bus=uas.0,scsi-id=0,lun=0,drive=disk1,rotation_rate=1


# full example
sudo ~/git/qemu/x86_64-softmmu/qemu-system-x86_64 -m 4G,slots=256,maxmem=128G \
    -smp 4,maxcpus=16 --enable-kvm \
    -drive file=~/guest/fedora31.img,format=raw -nographic \
    -drive file=~/guest/project.img,format=raw \
    -device nec-usb-xhci,id=xhci                                    \
    -device usb-uas,id=uas,bus=xhci.0                               \
    -drive if=none,id=disk1,file=~/guest/swap.img,format=raw \
    -device scsi-hd,bus=uas.0,scsi-id=0,lun=0,drive=disk1,rotation_rate=1
