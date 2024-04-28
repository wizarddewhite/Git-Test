#ifndef __LIB_H_
#define __LIB_H_

#define __section(section)              __attribute__((__section__(section)))

#define TRUE  1
#define FALSE 0

extern char __init_begin[], __init_end[];
char * itoa(char * str, unsigned long num);
void show_init_begin_end();

#endif //  __LIB_H_
