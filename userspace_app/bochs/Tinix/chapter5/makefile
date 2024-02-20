all: boot.img

boot.img: boot.bin loader.bin kernel.bin
	dd if=boot.bin of=boot.img
	dd if=/dev/zero of=boot.img seek=1 bs=512 count=2879

%.bin: %.asm
	nasm $^ -o $@

kernel.bin: kernel.asm
	nasm -f elf kernel.asm -o kernel.o
	ld -m elf_i386 -s kernel.o -o kernel.bin

.PHONY: run install
run:
	bochs -f bochsrc

install: all
	sudo mount boot.img /tmp -o loop
	sudo cp loader.bin /tmp/
	sudo cp kernel.bin /tmp/
	sudo umount /tmp

clean:
	rm -f boot.img *.bin *.o disboot.asm

disboot.asm:
	ndisasm -o 0x7c00 boot.bin >> disboot.asm

