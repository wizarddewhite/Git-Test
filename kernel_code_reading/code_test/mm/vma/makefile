SOURCES := $(wildcard *.c)
PROGRAM := main
OBJS := $(subst .c,.o,$(SOURCES))
LIB := ../../rbtree/lib/rb_tree.a
DEPENDENCY = $(subst .c,.d,$(SOURCES))
CFLAGS = -g -I../ -I../../rbtree/lib/
LD_FLAGS = -lm

#$(warning $(OBJS))

.PHONY: all clean cscope
all: $(LIB) $(PROGRAM)

main: main.c vma.c vma.h
	gcc $(CFLAGS) $(LD_FLAGS) $^ $(LIB) -o $@

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

%.a: %.o
	ar cr $@ $^

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
	rm -f $(LIB)

cscope:
	cscope -Rbkq
	ctags -R
