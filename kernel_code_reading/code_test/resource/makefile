SOURCES := $(wildcard *.c)
PROGRAM := $(basename $(SOURCES))
DEPENDENCY = $(subst .c,.d,$(SOURCES))
CFLAGS = -g -I../ -I../lib/

LIB_SOURCE := $(wildcard ../lib/*.c)
LIB := $(subst .c,.o,$(LIB_SOURCE))

#$(warning $(MAKECMDGOALS) $(PROGRAM))

.PHONY: all clean cscope
all: pci_resource_test

pci_resource_test: pci_resource_test.o $(LIB)
	gcc $(CFLAGS) -o $@ $^


%: %.o
	gcc $(CFLAGS) $<  -o $@ 

%.o : %.c
	gcc $(CFLAGS) -c $< -o $@

ifneq "$(MAKECMDGOALS)" "clean"
        include $(DEPENDENCY)
endif

%.d: %.c
	gcc -MM $(CFLAGS) $< > $@.$$$$ ;            \
	sed 's,/($*/)/.o[ :]*,/1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

clean:
	rm -f $(PROGRAM)
	rm -f $(DEPENDENCY)
	rm -f *.o
	rm -f cscope.*
	rm -f tags

cscope:
	cscope -Rbkq
	ctags -R
