PROGRAM := btree_test
PROGRAM_OBJS := $(addsuffix .o, $(PROGRAM))
SOURCES := $(wildcard *.c)
ALL_OBJS := $(subst .c,.o,$(SOURCES))
OBJS := $(filter-out $(PROGRAM_OBJS),$(ALL_OBJS))
LIB :=
DEPENDENCY = $(subst .c,.d,$(SOURCES))
CFLAGS = -g -I../ -Ilib/
LD_FLAGS = -lm

# $(warning $(PROGRAM_OBJS))
# $(warning $(OBJS))

.PHONY: all clean cscope
all: $(LIB) $(PROGRAM)

%: %.c $(OBJS)
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
