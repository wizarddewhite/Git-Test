SOURCES = $(wildcard *.c)
PROGRAM := $(basename $(SOURCES))

.PHONY: all clean cscope
all: server client cscope

session_client.o: session.o

client: client.c display.o command.o session.o session_client.o \
	net_connection.o tcp_connection_test.o
	gcc -Wall -g $^ -o $@ -lncurses

%.o : %.c %.h
	gcc -Wall -c $< -o $@ 

cscope:
	cscope -Rbkq

clean:
	rm -f *.o server client
	rm -f cscope*
