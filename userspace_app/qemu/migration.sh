# through tcp

## source
sudo ~/git/qemu/x86_64-softmmu/qemu-system-x86_64 -m 4G,slots=4,maxmem=128G -smp 4,maxcpus=16 --enable-kvm \
	-drive file=/home/richard/guest/fedora29.img,format=raw -nographic \
	-drive file=/home/richard/guest/project.img,format=raw \

## type in monitor
migrate -d tcp:0:4444

## destination
sudo ~/git/qemu/x86_64-softmmu/qemu-system-x86_64 -m 4G,slots=4,maxmem=128G -smp 4,maxcpus=16 --enable-kvm \
	-drive file=/home/richard/guest/fedora29.img,format=raw -nographic \
	-drive file=/home/richard/guest/project.img,format=raw \
	-incoming tcp:0:4444



# through exec

## source
sudo ~/git/qemu/x86_64-softmmu/qemu-system-x86_64 -m 4G,slots=4,maxmem=128G -smp 4,maxcpus=16 --enable-kvm \
	-drive file=/home/richard/guest/fedora29.img,format=raw -nographic \
	-drive file=/home/richard/guest/project.img,format=raw \

## type in monitor
stop
migrate "exec:cat > /your/back/file"
quit

## destination
sudo ~/git/qemu/x86_64-softmmu/qemu-system-x86_64 -m 4G,slots=4,maxmem=128G -smp 4,maxcpus=16 --enable-kvm \
	-drive file=/home/richard/guest/fedora29.img,format=raw -nographic \
	-drive file=/home/richard/guest/project.img,format=raw \
	-incoming "exec:cat < /your/back/file"

## type in monitor
cont

# free page opt

-object iothread,id=iothread1 --device virtio-balloon,free-page-hint=true,iothread=iothread1

# set migration capability
migrate_set_capability postcopy-ram on
; https://wiki.qemu.org/Features/AutoconvergeLiveMigration
migrate_set_capability auto-converge on

# post copy
https://wiki.qemu.org/Features/PostCopyLiveMigration

migrate_set_capability postcopy-ram on # both source and destination
migrate_set_capability postcopy-blocktime on # both source and destination
migrate -d tcp:0:4444
migrate_start_postcopy   # after first round of sync

