SOURCES := $(wildcard *.c)
PROGRAM := $(basename $(SOURCES))
DEPENDENCY = $(subst .c,.d,$(SOURCES))
CFLAGS = -g

#$(warning $(MAKECMDGOALS) $(PROGRAM))

.PHONY: all clean cscope
all: align_case queue_case cscope \
	map_case sort_case bitmap_case

queue_case: queue_case.c kfifo.c
	gcc -o $@ -g $^

map_case: map_case.c idr.c
	gcc -o $@ -g $^

sort_case: sort_case.c sort.c
	gcc -o $@ -g $^

bitmap_case: bitmap_case.c bitmap.h
	gcc -o $@ -g $^

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
