all: a.img

a.img: boot.asm
	nasm boot.asm -o boot.bin
	dd if=boot.bin of=a.img
	dd if=/dev/zero of=a.img seek=1 bs=512 count=2879

clean:
	rm -f a.img boot.bin
