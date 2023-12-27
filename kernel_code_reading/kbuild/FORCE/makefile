all: test

force_1.o: force_1.c FORCE
force_2.o: force_2.c
unforce.o: unforce.c

%.o: %.c
	gcc -c $< -o $@ 

test: force_1.o force_2.o unforce.o test.o
	gcc $^ -o $@

clean:
	rm *.o test

.PHONY: FORCE clean
FORCE:
