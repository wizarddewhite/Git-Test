#!/bin/bash
QEMU=qemu-system-x86_64
DISK="-drive file=/home/richard/guest/fedora.img"
DEFAULT="-m 4096 -smp 4 --enable-kvm "
NO_GRAPHIC=" "
SERIAL="-serial telnet:localhost:4321,server,nowait"
INSTALL=
MIGRATE=

while getopts ":siuvmkq" opt; do
	case "$opt" in
	"q")
		QEMU="/home/richard/git/qemu/x86_64-softmmu/qemu-system-x86_64"
		;;
	"k")
		# boot guest with kernel/initrd on host
		# append the kernel parameter in guest
		KERNEL="-kernel /boot/vmlinuz-4.7.0+ -initrd /boot/initrd.img-4.7.0+ -append 'root=UUID=3a9c5ef1-bbee-4387-8390-c02380bd177c  ro  quiet splash'"
		;;
	"m")
		MIGRATE="-incoming tcp:0:4444"
		;;
	"s")
		SERIAL="-serial telnet:localhost:4322,server,nowait"
		;;
	"i")
		INSTALL="-drive file=Fedora-Live-Workstation-x86_64-23-10.iso,media=cdrom \
		-append console=ttyS0 root=live:\
		-kernel tmp/isolinux/vmlinuz0 -initrd tmp/isolinux/initrd0.img"
		;;
	"u")
		DISK="-drive file=/home/richard/guest/ubuntu.img"
		;;
	"v")
		NO_GRAPHIC=" -nographic "
		;;
	":")
		echo "no argument for option: $OPTARG"
		;;
	"?")
		echo "not valid option: $OPTARG"
		;;
	esac
done

echo "run with -- " $QEMU $DEFAULT $NO_GRAPHIC $DISK $INSTALL $MIGRATE $KERNEL
#$QEMU $DEFAULT $NO_GRAPHIC $DISK $INSTALL $MIGRATE
