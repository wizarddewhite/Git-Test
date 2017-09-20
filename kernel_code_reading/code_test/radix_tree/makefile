SOURCES := $(wildcard *.c)
OBJS := $(subst .c,.o,$(SOURCES))
DEPENDENCY = $(subst .c,.d,$(SOURCES))
PROGRAM := radix_tree
CFLAGS := -g -Wall

.PHONY: all clean
all: $(PROGRAM)

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

$(PROGRAM): $(OBJS)
	gcc $(CFLAGS) $^ -o $@ 

ifneq "$(MAKECMDGOALS)" "clean"
        include $(DEPENDENCY)
endif

%.d: %.c
	gcc -MM $(CFLAGS) $< > $@.$$$$ ;            \
	sed 's,/($*/)/.o[ :]*,/1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

clean:
	rm -f $(PROGRAM) $(OBJS) $(DEPENDENCY)
