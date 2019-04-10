
/home/richard/git/qemu/x86_64-softmmu/qemu-system-x86_64 -machine pc,nvdimm \
	-m 4G,slots=4,maxmem=128G -smp 8 --enable-kvm \
	-drive file=/home/richard/guest/fedora29.img,format=raw -nographic \
	-drive file=/home/richard/guest/project.img,format=raw \
	-object memory-backend-file,id=mem1,share=on,mem-path=/dev/dax0.0,size=1G,align=2M \
	-device nvdimm,id=nvdimm1,memdev=mem1,label-size=128k \
	-object memory-backend-file,id=mem2,share=on,mem-path=/dev/dax0.1,size=2G,align=2M \
	-device nvdimm,id=nvdimm2,memdev=mem2,label-size=128k
