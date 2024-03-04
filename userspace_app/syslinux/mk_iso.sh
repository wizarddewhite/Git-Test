#!/bin/bash
#Create the Workspace folder in /var/tmp
rm -rf /var/tmp/Workspace/ISO/
mkdir -p /var/tmp/Workspace/ISO/isolinux
mkdir -p /var/tmp/Workspace/ISO/tools
mkdir -p /var/tmp/Workspace/ISO/images

#Get the files
if [ ! -f /var/tmp/Workspace/syslinux-6.03.tar.gz ]; then
	wget https://www.kernel.org/pub/linux/utils/boot/syslinux/syslinux-6.03.tar.gz -P /var/tmp/Workspace/
else
	echo syslinux already downloaded
fi

#Extract the files
rm -rf /var/tmp/Workspace/syslinux-6.03
tar -C /var/tmp/Workspace/ -xzf /var/tmp/Workspace/syslinux-6.03.tar.gz

#Copying require files
cp /var/tmp/Workspace/syslinux-6.03/bios/core/isolinux.bin /var/tmp/Workspace/ISO/isolinux/
cp /var/tmp/Workspace/syslinux-6.03/bios/com32/menu/menu.c32 /var/tmp/Workspace/ISO/isolinux/
cp /var/tmp/Workspace/syslinux-6.03/bios/com32/libutil/libutil.c32 /var/tmp/Workspace/ISO/isolinux/
cp /var/tmp/Workspace/syslinux-6.03/bios/com32/elflink/ldlinux/ldlinux.c32 /var/tmp/Workspace/ISO/isolinux/

cp /var/tmp/Workspace/syslinux-6.03/bios/com32/modules/reboot.c32 /var/tmp/Workspace/ISO/tools/
cp /var/tmp/Workspace/syslinux-6.03/bios/com32/modules/poweroff.c32 /var/tmp/Workspace/ISO/tools/
cp /var/tmp/Workspace/syslinux-6.03/bios/com32/lib/libcom32.c32 /var/tmp/Workspace/ISO/isolinux/

#Create empty configuration file
touch /var/tmp/Workspace/ISO/isolinux/isolinux.cfg

# prepare a simple initrd
gcc -static -o hello hello.c
echo hello | cpio -o --format=newc > /var/tmp/Workspace/ISO/images/rootfs
rm hello
cp ~/git/linux/arch/x86/boot/bzImage /var/tmp/Workspace/ISO/images/


#Set configuration file content
cat <<EOF > /var/tmp/Workspace/ISO/isolinux/isolinux.cfg
UI menu.c32

MENU TITLE "THIS IS A TEST"

LABEL TEST
	MENU LABEL Back to boot proccess
	MENU DEFAULT
	LOCALBOOT 0
	TEXT HELP
		Exit and continue normal boot
	ENDTEXT

LABEL RawKernel
	MENU LABEL ^RawKernel
	KERNEL /images/bzImage
	INITRD /images/rootfs
	APPEND root=/dev/ram rdinit=/hello
	TEXT HELP
		raw kenrel bootup
	ENDTEXT

LABEL Reboot
	MENU LABEL ^Reboot computer
	KERNEL /tools/reboot.c32
	TEXT HELP
		Reboot the computer (you can use CTRL+ALT+SUPPR too)
	ENDTEXT

LABEL Shutdown
	MENU LABEL ^Shutdown computer
	KERNEL /tools/poweroff.c32
	TEXT HELP
		Shutdown the computer
	ENDTEXT
EOF

# Create image.iso from ISO folder to Workspace
rm -f /var/tmp/Workspace/image.iso
genisoimage -rational-rock -volid "Testing ISO" -cache-inodes -joliet -full-iso9660-filenames \
-b isolinux/isolinux.bin -c isolinux/boot.cat \
-no-emul-boot -boot-load-size 4 -boot-info-table -input-charset UTF8 \
-output /var/tmp/Workspace/image.iso /var/tmp/Workspace/ISO/