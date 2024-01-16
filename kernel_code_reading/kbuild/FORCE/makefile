include Kbuild.include
include Kbuild
include Makefile.libs

all: force_test unforce_test

force.o: FORCE

%.o: %.c
	gcc -c $< -o $@ 

force_test: $(call real-search, force_test.o, .o, -objs)
	gcc $^ -o $@

unforce_test: $(call real-search, unforce_test.o, .o, -objs)
	gcc $^ -o $@

cmd_clean = echo cleanup build result

clean:
	$(call cmd,clean)
	@rm -f *.o force_test unforce_test

.PHONY: FORCE clean
