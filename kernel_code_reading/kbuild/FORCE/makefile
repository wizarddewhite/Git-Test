include Kbuild.include

all: force_test unforce_test

force.o: FORCE

%.o: %.c
	gcc -c $< -o $@ 

force_test: force.o unforce.o force_test.o
	gcc $^ -o $@

unforce_test: unforce.o unforce_test.o
	gcc $^ -o $@

cmd_clean = echo cleanup build result

clean:
	$(call cmd,clean)
	@rm -f *.o force_test unforce_test

.PHONY: FORCE clean
