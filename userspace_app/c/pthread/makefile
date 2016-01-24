SOURCES := $(wildcard *.cpp)
SOURCES += $(wildcard *.c)
PROGRAM := $(basename $(SOURCES))

.PHONY: all clean
all: $(PROGRAM)

%: %.cpp
	g++ -Wall -g $< -o $@ 

%: %.c
	gcc -Wall -g $< -o $@ -lpthread
clean:
	rm -f $(PROGRAM)
