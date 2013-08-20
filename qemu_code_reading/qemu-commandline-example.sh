# installing fedora on x86
#~/git/qemu/i386-softmmu/qemu-system-i386 -enable-kvm -drive file=~/git/testing/fedora_32.img -boot dc -m 1024 -monitor stdio -cdrom ~/isos/Fedora-15-i386-DVD.iso
#~/git/qemu/x86_64-softmmu/qemu-system-x86_64 -enable-kvm -drive file=~/git/testing/fedora_64.img -boot dc -m 1024 -monitor stdio #-cdrom ~/isos/Fedora-16-x86_64-DVD.iso

# run guest with dedicated kernel
#~/git/qemu/i386-softmmu/qemu-system-i386 -enable-kvm -drive file=~/git/testing/fedora.img -boot dc -m 1024 -monitor stdio -kernel /home/ywywyang/git/linux-next/arch/x86/boot/bzImage -append "root=/dev/sda3 rw"
#~/git/qemu/x86_64-softmmu/qemu-system-x86_64 -enable-kvm -drive file=~/git/testing/fedora_64.img -boot dc -m 1024 -monitor stdio  -kernel /home/ywywyang/git/linux-next/arch/x86/boot/bzImage -append "root=/dev/sda4 rw "


#install RHEL on ppc64
#/home/ywywyang/git/qemu-impreza/ppc64-softmmu/qemu-system-ppc64 -M pseries -m 1024 -enable-kvm -nographic -drive file=/home/ywywyang/rhel.img   -monitor telnet:localhost:5432,server,nowait -boot dc -cdrom /home/ywywyang/RHEL6.2-20111117.0-Server-ppc64-DVD1.iso 

#install FC16 on ppc64
#/home/ywywyang/git/qemu-impreza/ppc64-softmmu/qemu-system-ppc64 -M pseries -m 1024 -enable-kvm -nographic -drive file=/home/ywywyang/fc16.img -monitor telnet:localhost:5432,server,nowait -boot dc -cdrom /home/ywywyang/Fedora-16-ppc64-DVD.iso
#install FC16 on ppc64 with ks.iso
#/home/ywywyang/git/qemu-impreza/ppc64-softmmu/qemu-system-ppc64 -M pseries -m 1024 -enable-kvm -nographic -drive file=/home/ywywyang/fc16.img -monitor telnet:localhost:5432,server,nowait -boot dc -drive file=/home/ywywyang/Fedora-16-ppc64-DVD.iso,media=cdrom,index=1 -drive file=/home/ywywyang/kvm_autotest_root/images/f16-ppc64/ks.iso,media=cdrom,index=2 -kernel /home/ywywyang/kvm_autotest_root/images/f16-ppc64/vmlinuz -initrd /home/ywywyang/kvm_autotest_root/images/f16-ppc64/initrd.img -append "root=live:CDLABEL=Fedora-16-ppc64 ks=cdrom:/ks.cfg console=hvc0 serial rd_NO_PLYMOUTH"

#run the FC16 on ppc64
#/home/ywywyang/git/qemu-impreza/ppc64-softmmu/qemu-system-ppc64 -M pseries -m 1024 -enable-kvm -nographic -drive file=/home/ywywyang/fc16.img -monitor telnet:localhost:5432,server,nowait -boot cd 
#run the FC16 on ppc64 with tap
#/home/ywywyang/git/qemu-impreza/ppc64-softmmu/qemu-system-ppc64 -M pseries -m 1024 -enable-kvm -nographic -drive file=/home/ywywyang/fc16.img -monitor telnet:localhost:5432,server,nowait -netdev tap,id=tapnet,ifname=tap0,script=no -device spapr-vlan,netdev=tapnet -boot cd 
#run the FC16 on ppc64 with dedicated kernel
#/home/ywywyang/git/qemu-impreza/ppc64-softmmu/qemu-system-ppc64 -M pseries -m 1024 -enable-kvm -nographic -drive file=/home/ywywyang/fc16.img -monitor telnet:localhost:5432,server,nowait -boot cd -append "root=/dev/mapper/VolGroup-lv_root rd.md=0 rd.dm=0 rd.lvm.lv=VolGroup/lv_swap console=tty0  KEYTABLE=us console=ttyS0,115200 rd.lvm.lv=VolGroup/lv_root rd.luks=0 SYSFONT=latarcyrheb-sun16 LANG=en_US.UTF-8" \
#-kernel /home/ywywyang/vmlinuz-3.1.0-0.rc9.git0.2.fc16.kh.ppc64  \
#-initrd /home/ywywyang/initramfs-3.1.0-0.rc9.git0.2.fc16.kh.ppc64.img


#install FC17 on ppc64
#/home/ywywyang/git/qemu-impreza/ppc64-softmmu/qemu-system-ppc64 -M pseries -m 1024 -enable-kvm -nographic -drive file=/home/ywywyang/fc17.img -monitor telnet:localhost:5435,server,nowait -boot dc -cdrom /home/ywywyang/Fedora-17-ppc64-DVD.iso

#run the FC17 on ppc64 with tap
#/home/ywywyang/git/qemu-impreza/ppc64-softmmu/qemu-system-ppc64 -M pseries -m 1024 -enable-kvm -nographic -drive file=/home/ywywyang/fc17.img -monitor telnet:localhost:5435,server,nowait -netdev tap,id=tapnet,ifname=tap0,script=no -device spapr-vlan,netdev=tapnet -boot cd 

# use usb disk
#/home/ywywyang/git/qemu-impreza/ppc64-softmmu/qemu-system-ppc64 -M pseries -m 1024 -enable-kvm -nographic -device usb-ehci,id=usb1 -hda /home/ywywyang/kvm_autotest_root/images/f16-ppc64.qcow2 -drive if=none,id=usbstick,file='/home/ywywyang/usbdevice.img' -device usb-storage,bus=usb1.0,drive=usbstick 


# install FC19 on ppc64
# /home/ywywyang/git/qemu-impreza/ppc64-softmmu/qemu-system-ppc64 -M pseries -m 2048 -enable-kvm -nographic -drive file=/home/ywywyang/kvm/fc19.img -monitor telnet:localhost:5435,server,nowait -boot dc -cdrom /home/ywywyang/Fedora-19-ppc64-DVD.iso -netdev tap,id=tapnet,ifname=tap0,script=no -device rtl8139,netdev=tapnet,mac=96:83:DA:A0:06:34
