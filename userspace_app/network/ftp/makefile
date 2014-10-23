SOURCES := $(wildcard *.cpp)
SOURCES += $(wildcard *.c)
PROGRAM := $(basename $(SOURCES))

.PHONY: all clean
all: server client

client: client.c display.o command.o
	gcc -Wall -g $^ -o $@ -lncurses

%.o : %.c
	gcc -Wall -c $< -o $@ 

clean:
	rm -f *.o server client
