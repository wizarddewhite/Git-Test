SOURCES := $(wildcard *.cpp)
SOURCES += $(wildcard *.c)
PROGRAM := $(subst vm_util,,$(basename $(SOURCES)))

.PHONY: all clean
all: $(PROGRAM)

$(PROGRAM): vm_util.o

%.o: %.c
	gcc -c -o $@ $<

%: %.cpp
	g++ -Wall -g $< -o $@ 

%: %.c
	gcc -Wall -g $< -o $@ -lnuma -lpthread vm_util.o
clean:
	rm -f $(PROGRAM) vm_util.o
