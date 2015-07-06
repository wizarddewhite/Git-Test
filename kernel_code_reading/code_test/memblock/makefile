SOURCES := $(wildcard *.c)
PROGRAM := $(basename $(SOURCES))
DEPENDENCY = $(subst .c,.d,$(SOURCES))
CFLAGS = -g -I../

#$(warning $(MAKECMDGOALS) $(PROGRAM))

.PHONY: all clean cscope
all: main

main: main.o memblock.o
	gcc $(CFLAGS) -lm -o main main.o memblock.o

%: %.o
	gcc $(CFLAGS) $<  -o $@ 

%.o : %.c
	gcc $(CFLAGS) -lm -c $< -o $@

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
