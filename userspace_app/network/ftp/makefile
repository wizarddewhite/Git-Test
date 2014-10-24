SOURCES = $(wildcard *.c)
PROGRAM := $(basename $(SOURCES))

.PHONY: all clean
all: server client

session_client.o: session.o

client: client.c display.o command.o session.o \
	session_client.o
	gcc -Wall -g $^ -o $@ -lncurses

%.o : %.c %.h
	gcc -Wall -c $< -o $@ 

clean:
	rm -f *.o server client
