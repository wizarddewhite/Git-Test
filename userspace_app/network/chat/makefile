SOURCES := $(wildcard *.cpp)
SOURCES += $(wildcard *.c)
PROGRAM := $(basename $(SOURCES))

.PHONY: all clean
all: $(PROGRAM)

%: %.c
	gcc -Wall -g $< -o $@ -lpthread
clean:
	rm -f $(PROGRAM)
