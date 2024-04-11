#!/bin/bash
QEMU=/home/richard/git/qemu/build/qemu-system-x86_64
DISK="-drive file=/home/richard/guest/fedora.img,format=raw -drive file=/home/richard/guest/project.img,format=qcow2 "
DEFAULT="-m 4096 -smp 8 --enable-kvm "
NO_GRAPHIC="-nographic "
INSTALL=""
MIGRATE=""
KERNEL=""
IS_TRY=""
SERIAL=""

usage()
{
	echo "Usage: run a guest"
	echo "$0 [-hsiuvmnkq]"
	printf "\t-h this help message \n"
	printf "\t-s open serial port \n"
	printf "\t-v start vnc and \"change vnc password\" in monitor \n"
	printf "\t-m start as migration target \n"
	printf "\t-k \n"
	printf "\t-i re-install guest \n"
	printf "\t-t just print the qemu command line \n"
	exit
}

while getopts ":hsvmkit" opt; do
	case "$opt" in
	"h")
		usage
		;;
	"s")
		SERIAL="-serial telnet:localhost:4321,server,nowait "
		;;
	"i")
		INSTALL="-drive file=Fedora-Live-Workstation-x86_64-23-10.iso,media=cdrom \
		-append console=ttyS0 root=live:\
		-kernel tmp/isolinux/vmlinuz0 -initrd tmp/isolinux/initrd0.img"
		;;
	"v")
		NO_GRAPHIC="-vnc :0,password -monitor stdio"
		;;
	"m")
		MIGRATE="-incoming tcp:0:4444"
		;;
	"k")
		# boot guest with kernel/initrd on host
		# append the kernel parameter in guest
		KERNEL="-kernel /boot/vmlinuz-4.7.0+ -initrd /boot/initrd.img-4.7.0+ -append 'root=UUID=3a9c5ef1-bbee-4387-8390-c02380bd177c  ro  quiet splash'"
		;;
	"t")
		IS_TRY="true"
		;;
	":")
		echo "no argument for option: $OPTARG"
		;;
	"?")
		echo "not valid option: $OPTARG"
		;;
	esac
done

if [ "$IS_TRY" == "true" ]; then
	printf "%s \n" "$QEMU"
	printf "\t%s %s \n" "$DEFAULT" "$NO_GRAPHIC"
	printf "\t%s \n" "$DISK"
	if [ -n "$INSTALL" ]; then
		printf "aaa%sbbb \n" "$INSTALL"
	fi
	if [ -n "$MIGRATE" ]; then
		printf "\t%s \n" "$MIGRATE"
	fi
	if [ -n "$KERNEL" ]; then
		printf "\t%s \n" "$KERNEL"
	fi
	if [ -n "$SERIAL" ]; then
		printf "\t%s \n" "$SERIAL"
	fi
else
	echo Start quest...
	$QEMU $DEFAULT $NO_GRAPHIC $DISK $INSTALL $MIGRATE $KERNEL $SERIAL
fi
