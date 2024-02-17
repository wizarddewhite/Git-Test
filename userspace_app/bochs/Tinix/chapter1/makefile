all: boot.img

boot.img: boot.asm
	nasm boot.asm -o boot.bin
	dd if=boot.bin of=boot.img
	dd if=/dev/zero of=boot.img seek=1 bs=512 count=2879

.PHONY: run
run:
	bochs -f bochsrc

clean:
	rm -f boot.img boot.bin
