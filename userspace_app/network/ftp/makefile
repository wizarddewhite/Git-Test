SOURCES = $(wildcard *.c)
PROGRAM := $(basename $(SOURCES))

.PHONY: all clean
all: server client

client: client.c display.o command.o session.o
	gcc -Wall -g $^ -o $@ -lncurses

%.o : %.c %.h
	gcc -Wall -c $< -o $@ 

clean:
	rm -f *.o server client
