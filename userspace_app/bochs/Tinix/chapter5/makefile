all: hello

hello: hello.asm
	nasm -f elf hello.asm -o hello.o
	ld -m elf_i386 -s hello.o -o hello

.PHONY: run install

clean:
	rm -f hello *.o

