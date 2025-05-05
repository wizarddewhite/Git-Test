# install with console mode

sudo ~/git/qemu/x86_64-softmmu/qemu-system-x86_64 -m 8G -smp 8 --enable-kvm \
-drive file=/home/richard/guest/fedora29.img.back -nographic \
-drive file=/home/richard/guest/isos/Fedora-Server-dvd-x86_64-29-1.2.iso,media=cdrom \
-append console=ttyS0 -kernel /home/richard/guest/vmlinuz \
-initrd /home/richard/guest/initrd.img \


# normal install with vnc
# 如果是password,则需要在启动后，要在monitor中运行"change vnc password"来修改vnc密码才能成功连接。
# 如果是password=off，则不需要在monitor中设置密码了。
# 在mac上，直接CMD+Space，输入vnc://台式机的ip。台式机本身没有开vnc，默认虚拟机的vnc端口还是5900。
sudo ~/git/qemu/x86_64-softmmu/qemu-system-x86_64 -m 8G -smp 8 --enable-kvm \
-vnc :0,password=off -monitor stdio \
-drive file=/home/richard/guest/fedora29.img.back \
-drive file=/home/richard/guest/isos/Fedora-Server-dvd-x86_64-29-1.2.iso,media=cdrom \
-boot d
