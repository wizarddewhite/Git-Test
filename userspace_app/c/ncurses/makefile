SOURCES := $(wildcard *.cpp)
SOURCES += $(wildcard *.c)
PROGRAM := $(basename $(SOURCES))

.PHONY: all clean
all: $(PROGRAM)

%: %.cpp
	g++ -Wall -g $< -o $@ -lncurses

%: %.c
	gcc -Wall -g $< -o $@ -lncurses -lm
clean:
	rm -f $(PROGRAM)
