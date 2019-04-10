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

file vmlinux
target remote :1234
set breakpoint pending on
set auto-load safe-path /home/richard/git/linux
hb start_kernel

