all: test

force.o: force.c FORCE
unforce.o: unforce.c

%.o: %.c
	gcc -c $< -o $@ 

test: force.o unforce.o test.o
	gcc $^ -o $@

clean:
	rm *.o test

.PHONY: FORCE clean
FORCE:
