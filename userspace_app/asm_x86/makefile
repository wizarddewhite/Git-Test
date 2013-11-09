SOURCES := $(wildcard *.cpp)
SOURCES += $(wildcard *.c)
PROGRAM := inline_asm hello-ia32

.PHONY: all clean
all: $(PROGRAM)

inline_asm: inline_asm.c asm_string.c asm_math.c
	gcc -Wall -g $^ -o $@

%: %.cpp
	g++ -Wall -g $< -o $@ 

%: %.s
	gcc -Wall -g $< -o $@

%: %.c
	gcc -Wall -g $< -o $@ 

clean:
	rm -f $(PROGRAM)
