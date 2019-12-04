SOURCES := $(wildcard *.c)
PROGRAM := main split_mem_range_refine
FOBJS := main.o split_mem_range_refine.o
OBJS := $(filter-out $(FOBJS), $(subst .c,.o,$(SOURCES)))
DEPENDENCY = $(subst .c,.d,$(SOURCES))
CFLAGS = -g -I../
LD_FLAGS = -lm

#$(warning $(OBJS))

.PHONY: all clean cscope
all: $(PROGRAM)

# for study split_mem_range()
main: main.c $(OBJS)
	gcc $(CFLAGS) $^ -o $@

# compare the effects of different split_mem_range()
split_mem_range_refine: split_mem_range_refine.c
	gcc $(CFLAGS) $^ -o $@

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
