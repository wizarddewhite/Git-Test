all: boot.img

boot.img: boot.bin
	dd if=boot.bin of=boot.img
	dd if=/dev/zero of=boot.img seek=1 bs=512 count=2879

boot.bin: boot.asm
	nasm boot.asm -o boot.bin

.PHONY: run
run:
	bochs -f bochsrc

clean:
	rm -f boot.img boot.bin disboot.asm

disboot.asm:
	ndisasm -o 0x7c00 boot.bin >> disboot.asm

