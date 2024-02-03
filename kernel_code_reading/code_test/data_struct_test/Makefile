ifeq ($(MIX),1)
PROGRAM := align_test queue_test \
	sort_test \
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

LIB_PATH := ../lib
LIB_SOURCE := $(wildcard $(LIB_PATH)/*.c)
LIB_OBJS := $(subst .c,.o,$(LIB_SOURCE))
LIB := $(LIB_PATH)/libtest.a
DEPENDENCY = $(subst .c,.d,$(SOURCES))
CFLAGS = -I../ -I$(LIB_PATH)/
LD_FLAGS = -lm

ifeq ($(DEBUG),1)
DEBUG_FLAGS = -g -DDEBUG
endif

# $(warning $(PROGRAM_OBJS))
# $(warning $(LIB_SOURCE))

.PHONY: all clean cscope
all: $(LIB) $(PROGRAM)

%: %.o $(LIB)
	@echo [GEN] $@
	@gcc $(DEBUG_FLAGS) $(LD_FLAGS) $^ -o $@

%.o: %.c
	@gcc $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

%.a: $(LIB_OBJS)
	@echo [AR] $@
	@ar cr $@ $^

ifneq "$(MAKECMDGOALS)" "clean"
        include $(DEPENDENCY)
endif

%.d: %.c
	@gcc -MM $(CFLAGS) $< > $@.$$$$ ;            \
	sed 's,/($*/)/.o[ :]*,/1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

clean:
	@echo [Clean] program/lib/prereques/cscope
	@rm -f $(PROGRAM)
	@rm -f $(DEPENDENCY)
	@rm -f *.o
	@rm -f cscope.*
	@rm -f $(LIB_OBJS) $(LIB)

cscope:
	cscope -Rbkq
	ctags -R
