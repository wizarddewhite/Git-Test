/*
 * =====================================================================================
 *
 *       Filename:  hlist_case.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/15/2013 02:37:26 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "hlist_case.h"

void hlist_init()
{
	int i;
	struct hlist_head *hash;
	int *p;

	hash = malloc(sizeof(*hash) * HASHENTRIES);

	if (!hash)
		return;

	/* Prove that INIT_HLIST_HEAD do is clean the hash array */
	p = (int*)hash;
	for (i = 0; i< HASHENTRIES; i++)
		p[i] = i;

	for (i = 0; i< HASHENTRIES; i++)
		INIT_HLIST_HEAD(&hash[i]);
}

int main()
{
	hlist_init();
	return 0;
}
