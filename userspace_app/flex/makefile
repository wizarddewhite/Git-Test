SOURCES := $(wildcard *.l)
PROGRAM := $(basename $(SOURCES))
MIDDLE = $(addsuffix .c, $(PROGRAM))

all: $(PROGRAM)

.SECONDARY: 

clean:
	rm -f $(MIDDLE)
	rm -f $(PROGRAM)

%: %.c
	gcc -o $@ $< -lfl

%.c: %.l
	flex -o $@ $<
