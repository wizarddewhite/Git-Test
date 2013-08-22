/*
 * =====================================================================================
 *
 *       Filename:  pci_resource_test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/14/2013 11:29:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <string.h>

#include "resource.h"
#include "resource_case.h"
#include "resource_request_simulation.h"
#include "__ffs.h"

#define LAST_CHILD_INDEX 4

struct resource root;
struct resource res[10];

void init3()
{
    root.start = 0;
    root.end   = 20 * (1 << 20); // 20M

    /* first child  */
    res[0].start = 0;
    res[0].end   = 1*(1<<20);

    /* res[1] is sibling of res[0], parent of all children*/
    /* Assign res from 2-9, and from big to small */
    res[2].start = 0;
    res[2].end = 8*(1<<20)-1; //8M
    res[3].start = 0x500000;
    res[3].end = res[3].start + 4*(1<<20)-1; //4M
    res[4].start = 0; //1M
    res[4].end = (1<<20)-1;

    insert_resource_conflict(&root, &res[0]);

    printf("initial version of resource list\n");
    dump2(&root, 0);
}


void mem_align_test()
{
	int max_order;
	struct resource_constraint constraint;
	resource_size_t aligns[12]; /* Alignments from 1Mb to 2Gb */
	resource_size_t min_align;
	resource_size_t size;
	int ret, i;

	init3();

	memset(aligns, 0, sizeof(aligns));
	max_order = 0;

	for (i = 2; i <= LAST_CHILD_INDEX; i++) {
		aligns[__ffs(resource_size(&res[i]))-20] += resource_size(&res[i]);
	}

	max_order = __ffs(resource_size(&res[2])) - 20;
	printf("max_order is %d\n", max_order);

	min_align = calculate_mem_align(aligns, max_order);
	printf("min_align is %08llx\n", (unsigned long long)min_align);

	for (i = 2, size = 0; i<= LAST_CHILD_INDEX; i++) {
		size += resource_size(&res[i]);
	}
	size = ALIGN(size, min_align);
	printf("size is %08llx\n", (unsigned long long)size);

	constraint.min = root.start;
	constraint.max = root.end;
	constraint.align = min_align;
	constraint.alignf = simple_align_resource;
	constraint.alignf_data = NULL;

	/* Allocate the parent resource as res[1] */
	ret = find_resource(&root, &res[1], size, &constraint);
	if (ret == 0)
	{
		printf("We find a size %08llx free slot under root at %08llx-%08llx\n",
			(unsigned long long)resource_size(&res[1]), 
			(unsigned long long)res[1].start, 
			(unsigned long long)res[1].end);
		request_resource_conflict(&root, &res[1]);
		dump2(&root, 0);
	}
	else {
		printf("We don't find a free slot under root for size 14M\n");
		return;
	}

	/* allocate from big to small */
	for (i = 2; i <= LAST_CHILD_INDEX; i++) {
		constraint.align = resource_size(&res[i]);
		ret = find_resource(&res[1], &res[i], resource_size(&res[i]), &constraint);
		if (ret == 0)
		{
			printf("We find a size %08llx free slot under root at %08llx-%08llx\n",
				(unsigned long long)resource_size(&res[i]), 
				(unsigned long long)res[i].start, 
				(unsigned long long)res[i].end);
			request_resource_conflict(&res[1], &res[i]);
			dump2(&root, 0);
		}
		else {
			printf("We don't find a free slot under root for size %08llx\n",
					(unsigned long long)resource_size(&res[i]));
			return;
		}
	}

	return;
}

int pci_get_max_depth(void)
{
	printf("depth is %d \n", pci_bus_get_depth2(root_bus));

	printf("depth is %d \n", pci_bus_get_depth(root_bus));

	return 0;
}

int main()
{
	mem_align_test();
	return 0;
}
