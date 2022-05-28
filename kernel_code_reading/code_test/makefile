ifeq ($(MIX),1)
PROGRAM := align_test queue_test \
	map_test sort_test \
	container_of_test percpu_test \
	bitmap_test btree_test maple_tree_test
PROGRAM_OBJS := $(addsuffix .o, $(PROGRAM))
SOURCES := $(wildcard *.c)
ALL_OBJS := $(subst .c,.o,$(SOURCES))
OBJS := $(filter-out $(PROGRAM_OBJS),$(ALL_OBJS))
else
SOURCES := $(wildcard *.c)
PROGRAM := $(subst .c,,$(SOURCES))
endif

LIB_SOURCE := $(wildcard lib/*.c)
LIB := $(subst .c,.o,$(LIB_SOURCE))
DEPENDENCY = $(subst .c,.d,$(SOURCES))
CFLAGS = -I./ -Ilib/
LD_FLAGS = -lm

ifeq ($(DEBUG),1)
DEBUG_FLAGS = -g -DDEBUG
endif

# $(warning $(PROGRAM_OBJS))
$(warning $(LIB_SOURCE))

.PHONY: all clean cscope
all: $(LIB) $(PROGRAM)

%: %.o $(LIB)
	gcc $(CFLAGS) $(DEBUG_FLAGS) $(LD_FLAGS) $^ -o $@

%.o: %.c
	gcc $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

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
