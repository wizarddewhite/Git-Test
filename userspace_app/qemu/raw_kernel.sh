# start raw kernel with customized initrd
# where rootfs is built from hello.c. Read hello.c for usage
# in qemu monitor, use "change vnc password"
sudo /home/richard/git/qemu/build/qemu-system-x86_64  \
  -m 4G -smp 8 --enable-kvm \
  -kernel /home/richard/git/linux/arch/x86/boot/bzImage \
  -initrd ./rootfs -append "root=/dev/ram rdinit=/hello" \
  -vnc :0,password -monitor stdio
