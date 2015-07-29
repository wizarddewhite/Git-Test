SOURCES := $(wildcard *.c)
PROGRAM := $(basename $(SOURCES))
OBJS := $(subst .c,.o,$(SOURCES))
DEPENDENCY = $(subst .c,.d,$(SOURCES))
CFLAGS = -g -I../
LD_FLAGS = -lm

#$(warning $(OBJS))

.PHONY: all clean cscope
all: main

main: $(OBJS)
	gcc $(CFLAGS) $(LD_FLAGS) -o main $(OBJS)

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
