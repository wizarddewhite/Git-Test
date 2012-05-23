all: test

CC ?= powerpc64-linux-gcc
CPPFLAGS = -m64

%.s : %.c
	$(CC) $(CPPFLAGS) -S -c $< -o $@

test: main.o foo-asm.o
	$(CC) $(CPPFLAGS) -O2 -o $@ $^

clean:
	rm -rf *.o *.s test
distclean: clean
	rm -rf *~
