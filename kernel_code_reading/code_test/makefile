SOURCES := $(wildcard *.c)
PROGRAM := $(basename $(SOURCES))
DEPENDENCY = $(subst .c,.d,$(SOURCES))
CFLAGS = -g

#$(warning $(MAKECMDGOALS) $(PROGRAM))

.PHONY: all clean cscope
all: resource

# this program emulate the kernel/resource.c to 
# get more understanding on this structrure
resource: resource.c resource_case.c
	gcc -o resource -g resource.c resource_case.c

%: %.c
	echo test
	gcc $(CFLAGS)  $<  -o $@ 

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
	rm -f cscope.*
	rm -f tags

cscope:
	cscope -Rbkq
	ctags -R
