SOURCES := $(wildcard *.c)
PROGRAM := $(basename $(SOURCES))
OBJS := $(subst .c,.o,$(SOURCES))
LIB := rb_tree.a
DEPENDENCY = $(subst .c,.d,$(SOURCES))
CFLAGS = -g -I../
LD_FLAGS = -lm

#$(warning $(OBJS))

.PHONY: all clean cscope
all: $(PROGRAM)

%: %.o $(LIB)
	gcc $(CFLAGS) $(LD_FLAGS) $^ -o $@

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
