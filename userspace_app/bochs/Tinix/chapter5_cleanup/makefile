all: boot.img

boot.img: boot.bin loader.bin kernel.bin
	dd if=boot.bin of=boot.img
	dd if=/dev/zero of=boot.img seek=1 bs=512 count=2879

%.bin: %.asm
	nasm $^ -o $@

%.o: %.asm
	nasm -f elf $^ -o $@

%.o: %.c
	gcc -m32 -c $^ -o $@

kernel.bin: kernel.o string.o klib.o start.o
	ld -Ttext 0x30400 -m elf_i386 -s $^ -o kernel.bin

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

