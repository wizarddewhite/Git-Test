all: hello foobar

hello: hello.asm
	nasm -f elf hello.asm -o hello.o
	ld -m elf_i386 -s hello.o -o hello

foobar: foo.asm bar.c
	nasm -f elf foo.asm -o foo.o
	gcc -m32 -c bar.c -o bar.o
	ld -m elf_i386 -s foo.o bar.o -o foobar

.PHONY: run install

clean:
	rm -f hello foobar *.o

