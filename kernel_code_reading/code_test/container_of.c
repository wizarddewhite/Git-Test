/*
 * =====================================================================================
 *
 *       Filename:  container_of.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/23/2013 10:50:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include "kernel.h"

struct test {
	int a;
	int b;
	int *c;
};

int main()
{
	struct test block;
	struct test *pb;
	int *p, **pp;
	int val;

	val = 8;
	block.a = 1;
	block.b = 2;
	block.c = &val;

	p = &block.a;
	pb = container_of(p, struct test, a);
	printf("block.a %d, block.b %d block.c %d\n",
			pb->a, pb->b, *pb->c);

	/* This is not correct */
	pp = &block.c;
	pb = container_of(pp, struct test, c);
	printf("block.a %d, block.b %d block.c %d\n",
			pb->a, pb->b, *pb->c);

	/* This is not correct */
	p = block.c;
	pb = container_of(&p, struct test, c);
	printf("block.a %d, block.b %d block.c %d\n",
			pb->a, pb->b, *pb->c);

	return 0;
}
