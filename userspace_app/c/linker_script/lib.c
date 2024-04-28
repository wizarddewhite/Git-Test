#include "lib.h"

extern int print(char *msg);

void __section(".meminit.text") test_func()
{
	print("A test function\n");

	show_init_begin_end();
}

void show_init_begin_end()
{
	char buf[128];

	print("__init_begin: ");
	itoa(buf, (unsigned long)&__init_begin);
	print(buf);
	print("\n");

	print("__init_end:   ");
	itoa(buf, (unsigned long)&__init_end);
	print(buf);
	print("\n");

}

/* 数字前面的 0 不被显示出来, 比如 0000B800 被显示成 B800 */
char * itoa(char * str, unsigned long num)
{
	char *	p = str;
	char	ch;
	int	i;
	int	flag = FALSE;

	*p++ = '0';
	*p++ = 'x';

	if(num == 0){
		*p++ = '0';
	}
	else{	
		for(i=60;i>=0;i-=4){
			ch = (num >> i) & 0xF;
			if(flag || (ch > 0)){
				flag = TRUE;
				ch += '0';
				if(ch > '9'){
					ch += 7;
				}
				*p++ = ch;
			}
		}
	}

	*p = 0;

	return str;
}
