# debug qemu, qemu's gdbinit
source scripts/qemu-gdb.py
file x86_64-softmmu/qemu-system-x86_64
handle SIGUSR1 nostop noprint
set args -nographic -machine pc -m 4G -smp 8 --enable-kvm -drive file=/home/richard/guest/fedora29.img,format=raw -drive file=/home/richard/guest/project.img,format=raw  -monitor telnet:127.0.0.1:55555,server,nowait
source gdb-script
b rom_add_blob
r

# debug guest kernel, qemu's gdbinit
source scripts/qemu-gdb.py
file x86_64-softmmu/qemu-system-x86_64
handle SIGUSR1 nostop noprint
set args -nographic -machine pc -m 4G -smp 8 --enable-kvm -drive file=/home/richard/guest/fedora29.img,format=raw -drive file=/home/richard/guest/project.img,format=raw  -monitor telnet:127.0.0.1:55555,server,nowait -s -S
source gdb-script
b rom_add_blob
r

# guest kernel preparation
# refer to
# https://yulistic.gitlab.io/2018/12/debugging-linux-kernel-with-gdb-and-qemu/
# https://unix.stackexchange.com/questions/396013/hardware-breakpoint-in-gdb-qemu-missing-start-kernel
#
# 1. Build kernel with CONFIG_DEBUG_INFO=y and CONFIG_RANDOMIZE_BASE=n
# 2. Make symbol file: objcopy --only-keep-debug vmlinux kernel.sym, and copy to host
# run gdb with following commands

file kernel.sym
target remote :1234
set breakpoint pending on
hb start_kernel

