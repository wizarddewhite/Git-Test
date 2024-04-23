all: main_sections main_normal
main_sections:
	gcc -ffunction-sections -fdata-sections -c main.c -o main_sections.o
	gcc -Wl,--gc-sections -o $@ main_sections.o

main_normal:
	gcc -c main.c -o main_normal.o
	gcc -o $@ main_normal.o

clean:
	rm -f *.o main_sections main_normal

sections_diff: all
	readelf -t main_sections.o | grep -A3 text
	readelf -t main_normal.o | grep -A3 text

symbol_diff: all
	readelf -s main_sections | grep func
	readelf -s main_normal | grep func
