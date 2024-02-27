ENTRYPOINT	= 0x30400
OBJS		= kernel/kernel.o kernel/start.o kernel/i8259.o \
		  kernel/global.o kernel/protect.o kernel/main.o \
		  kernel/clock.o \
		  lib/klib.o lib/klibc.o lib/string.o

all: boot/boot.img

boot/boot.img: boot/boot.bin boot/loader.bin kernel.bin
	dd if=$< of=$@
	dd if=/dev/zero of=$@ seek=1 bs=512 count=2879

%.bin: %.asm
	nasm -I boot/include $^ -o $@

%.o: %.asm
	nasm -I include -f elf $^ -o $@

%.o: %.c
	gcc -I include -fno-builtin -fno-stack-protector -m32 -c $^ -o $@

kernel.bin: $(OBJS)
	ld -Ttext $(ENTRYPOINT) -m elf_i386 -s $^ -o $@

.PHONY: run install
run:
	bochs -f bochsrc

install: all
	sudo mount boot/boot.img /tmp -o loop
	sudo cp boot/loader.bin /tmp/
	sudo cp kernel.bin /tmp/
	sudo umount /tmp

clean:
	rm -f boot/boot.img boot/*.bin
	rm -f kernel.bin $(OBJS)
	rm -f disboot.asm

disboot.asm:
	ndisasm -o 0x7c00 boot.bin >> disboot.asm

