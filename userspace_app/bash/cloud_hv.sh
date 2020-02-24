~/git/cloud-hypervisor/target/debug/cloud-hypervisor \
	--kernel ~/guest/hypervisor-fw \
	--disk ~/guest/clear-29160-kvm.img \
	--cpus 4 \
	--memory 512 \
	--net "tap=tap0,mac=,ip=192.168.122.100,mask=255.255.255.0" \
	--rng


~/git/cloud-hypervisor/target/debug/cloud-hypervisor \
	--kernel ~/guest/vmlinux.bin \
	--disk ~/guest/clear-29160-kvm.img \
	--cmdline "console=ttyS0 reboot=k panic=1 nomodules i8042.noaux i8042.nomux i8042.nopnp i8042.dumbkbd root=/dev/vda3" \
	--cpus 4 \
	--memory 512 \
	--net "tap=tap0,mac=,ip=192.168.122.100,mask=255.255.255.0" \
	--rng \
	--pmem "file=/home/richard/guest/2G-file,size=1073741824"
	--pmem "file=/dev/dax0.0,size=1024"


~/git/cloud-hypervisor/target/debug/cloud-hypervisor \
	--kernel ~/guest/vmlinux.bin \
	--disk ~/guest/fedora29.img \
	--cmdline "console=ttyS0 reboot=k panic=1 nomodules i8042.noaux i8042.nomux i8042.nopnp i8042.dumbkbd root=/dev/sda3" \
	--cpus 4 \
	--memory 512 \
	--net "tap=,mac=,ip=,mask=" \
	--rng 
