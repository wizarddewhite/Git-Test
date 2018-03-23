SOURCES := $(wildcard *.c)
PROGRAM := $(basename $(SOURCES))
DEPENDENCY = $(subst .c,.d,$(SOURCES))
CFLAGS = -g -I../ -I../list/

#$(warning $(MAKECMDGOALS) $(PROGRAM))

.PHONY: all clean cscope
all: resource 

# this program emulate the kernel/resource.c to 
# get more understanding on this structrure
resource: resource.c resource_case.o
	gcc $(CCFLAGS) -o resource resource.c resource_case.o

pci_resource_test: resource.o pci_resource_test.o resource_request_simulation.o
	gcc $(CCFLAGS) -o $@ $^


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
