SOURCES := $(wildcard *.cpp)
SOURCES += $(wildcard *.c)
PROGRAM := $(basename $(SOURCES))

.PHONY: all clean
all: inline_asm

inline_asm: inline_asm.c asm_string.c asm_math.c
	gcc -Wall -g $^ -o $@

%: %.cpp
	g++ -Wall -g $< -o $@ 

%: %.c
	gcc -Wall -g $< -o $@ 
clean:
	rm -f $(PROGRAM)
