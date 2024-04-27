LIB_PATH=/usr/lib/x86_64-linux-gnu/
DEF_LIBS=ld-linux-x86-64.so.2 crt1.o crti.o crtn.o libc.so
LIBS=$(addprefix ${LIB_PATH}, ${DEF_LIBS})

.PHONY: all clean
all: main

main: main.c
	@gcc -c main.c
	ld -T default.lds -dynamic-linker ${LIBS} main.o -o main

clean:
	rm -f main main.o
