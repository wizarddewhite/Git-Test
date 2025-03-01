PROGRAM := MemoryLeak OverFlow UseAfterFree
ASAN_FLAG := -fsanitize=address -fsanitize=undefined

.PHONY: all clean
all: $(PROGRAM)

%: %.cpp
	g++ -Wall -g $< -o $@ 

%: %.c
	gcc -Wall $(ASAN_FLAG) -g $< -o $@ 
clean:
	rm -f $(PROGRAM)
