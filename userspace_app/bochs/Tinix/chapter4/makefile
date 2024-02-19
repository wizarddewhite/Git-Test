all: boot.img

boot.img: boot.bin loader.bin
	dd if=boot.bin of=boot.img
	dd if=/dev/zero of=boot.img seek=1 bs=512 count=2879

boot.bin: boot.asm
	nasm boot.asm -o boot.bin

loader.bin: loader.asm
	nasm loader.asm -o loader.bin

.PHONY: run install
run:
	bochs -f bochsrc

install: all
	sudo mount boot.img /tmp -o loop
	sudo cp loader.bin /tmp/
	sudo umount /tmp

clean:
	rm -f boot.img boot.bin disboot.asm loader.bin

disboot.asm:
	ndisasm -o 0x7c00 boot.bin >> disboot.asm

