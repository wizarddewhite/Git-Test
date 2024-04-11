#!/bin/bash
QEMU=/home/richard/git/qemu/x86_64-softmmu/qemu-system-x86_64
DISK="-drive file=/home/richard/guest/ubuntu.img"
DEFAULT="-m 1024 -smp 2 --enable-kvm -nographic "
INSTALL=

while getopts ":si" opt; do
	case "$opt" in
	"s")
		QEMU=qemu-x86_64
		;;
	"i")
		INSTALL="-cdrom rhel-server-6.7-x86_64-dvd.iso -append console=ttyS0 \
		-kernel tmp/isolinux/vmlinuz -initrd tmp/isolinux/initrd.img"
		;;
	":")
		echo "no argument for option: $OPTARG"
		;;
	"?")
		echo "not valid option: $OPTARG"
		;;
	esac
done

echo "run with -- " $QEMU $DEFAULT $DISK $INSTALL
$QEMU $DEFAULT $DISK $INSTALL
