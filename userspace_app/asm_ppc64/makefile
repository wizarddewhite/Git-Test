SOURCES := $(wildcard *.cpp)
SOURCES += $(wildcard *.c)
PROGRAM := foo bar

.PHONY: all clean
all: $(PROGRAM)

%: %.s
	gcc -Wall -g $< -o $@

%: %.c
	gcc -Wall -g $< -o $@ 

clean:
	rm -f $(PROGRAM)
