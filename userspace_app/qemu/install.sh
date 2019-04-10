# install with console mode

sudo ~/git/qemu/x86_64-softmmu/qemu-system-x86_64 -m 8G -smp 8 --enable-kvm \
-drive file=/home/richard/guest/fedora29.img.back -nographic \
-drive file=/home/richard/guest/isos/Fedora-Server-dvd-x86_64-29-1.2.iso,media=cdrom \
-append console=ttyS0 -kernel /home/richard/guest/vmlinuz \
-initrd /home/richard/guest/initrd.img \


# normal install with vnc
sudo ~/git/qemu/x86_64-softmmu/qemu-system-x86_64 -m 8G -smp 8 --enable-kvm \
-drive file=/home/richard/guest/fedora29.img.back \
-drive file=/home/richard/guest/isos/Fedora-Server-dvd-x86_64-29-1.2.iso,media=cdrom 
