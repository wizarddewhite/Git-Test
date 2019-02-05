/*
 * =====================================================================================
 *
 *       Filename:  resource_case.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/16/2012 11:34:05 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include "resource.h"
#include "resource_case.h"
#include "resource_request_simulation.h"

struct resource root;
struct resource res[10];

void init()
{
    int i;

    root.start = 0;
    root.end   = 1000;

    res[0].start = 10;
    res[0].end   = 50;
    
    res[1].start = 90;
    res[1].end   = 150;
    
    res[2].start = 210;
    res[2].end   = 250;

    res[3].start = 310;
    res[3].end   = 450;

    /*link it*/
    root.child = &res[0];

    for(i = 0; i< 3; i++)
    {
        res[i].sibling = &res[i+1];
	res[i].parent = &root;
	res[i].child = NULL;
    }

    res[3].sibling = NULL;

    res[4].start = 90;
    res[4].end = 120;
    res[4].sibling = NULL;
    res[4].child = NULL;
    res[4].parent = &res[1];

    res[1].child = &res[4];

}

void init2()
{
    int i;

    root.start = 0;
    root.end   = 1000;

    res[0].start = 10;
    res[0].end   = 50;
    
    res[1].start = 90;
    res[1].end   = 150;
    
    res[2].start = 210;
    res[2].end   = 250;

    res[3].start = 310;
    res[3].end   = 450;

    res[4].start = 105;
    res[4].end = 140;

    for(i = 0; i< 5; i++)
    {
	    insert_resource_conflict(&root, &res[i]);
    }
    printf("initial version of resource list\n");
    dump(&root, 0);

    
}

void init3()
{
    int i;

    root.start = 0;
    root.end   = 1000;

    res[0].start = 10;
    res[0].end   = 50;
    
    res[1].start = 90;
    res[1].end   = 150;
    
    res[2].start = 210;
    res[2].end   = 250;

    res[3].start = 310;
    res[3].end   = 450;

    res[4].start = 105;
    res[4].end = 140;

    for(i = 0; i< 5; i++)
    {
	    insert_resource_conflict(&iomem_resource, &res[i]);
    }
    printf("initial version of resource list\n");
    dump(&iomem_resource, 0);

    
}
void insert_resource_test()
{
	init2();
	struct resource res1;
	res[6].start = 115;
	res[6].end = 140;
	res[6].parent = NULL;
	res[6].sibling = NULL;
	res[6].child = NULL;

	printf("insert a res to tree\n");
	insert_resource_conflict(&root, &res[6]);
	dump(&root, 0);

	printf("insert a large one which cover several res\n");
	res[5].start = 90;
	res[5].end = 260;
	res[5].parent = NULL;
	res[5].sibling = NULL;
	insert_resource_conflict(&root, &res[5]);
	dump(&root, 0);
	return;
}

//realloc_test(&root, res);
void realloc_test(struct resource *root, struct resource *res)
{
    int index;
    init();

    /*test cases for resoures*/
    dump(root, 0);
    struct resource new;
    scanf("%d", &index);
    /* 10-50, 90-150, 210-250, 310-450 */
    switch (index)
    {
        case 1: // expand the range, left side
            new.start = 5;
            new.end   = 50;
            __re_request_resource(root, &new, &res[0]);
            break;
        case 2: // expand the range, right side
            new.start = 10;
            new.end   = 60;
            __re_request_resource(root, &new, &res[0]);
            break;
        case 3: // expand the range, both side
            new.start = 7;
            new.end   = 80;
            __re_request_resource(root, &new, &res[0]);
            break;
        case 4: // allocate a new range, ahead of old
            new.start = 60;
            new.end   = 80;
            __re_request_resource(root, &new, &res[1]);
            break;
        case 5: // allocate a new range, after old
            new.start = 160;
            new.end   = 200;
            __re_request_resource(root, &new, &res[1]);
            break;
        case 6: // allocate a new range, at the beginning
            new.start = 2;
            new.end   = 8;
            __re_request_resource(root, &new, &res[2]);
            break;
        case 7: // allocate a new range, at last
            new.start = 500;
            new.end   = 600;
            __re_request_resource(root, &new, &res[2]);
            break;
        case 8: // allocate a new range, at last with old is the last too
            new.start = 500;
            new.end   = 600;
            __re_request_resource(root, &new, &res[3]);
            break;
        default:
            for (index = 0; index < 3; index++)
                printf("%lx\n", (unsigned long)&res[index]);
            printf("------------\n");
            remove_old(root, &res[2]);
            break;
    }

    dump(root, 0);

    return;
}

void relation_test()
{
	struct resource res1, res2;

	res1.start = 5;
	res1.end = 10;

	res2.start = 5;
	res2.end = 10;

	printf("res the same 1:%d\n", res1_contains_res2(&res1, &res2));

	res2.start = 6;
	res2.end = 10;
	printf("res1 contain res2 1:%d\n", res1_contains_res2(&res1, &res2));

	res2.start = 5;
	res2.end = 11;
	printf("res1 contain res2 0:%d\n", res1_contains_res2(&res1, &res2));

	printf("res contain 1:%d\n", resource_contains(&res1, &res2));

}

void release_test()
{
	init2();

	printf("release one important child\n");
	release_child_resources(&res[5]);
	release_resource(&res[5]);

	dump(&root, 0);
}

void find_resource_test()
{
	struct resource_constraint constraint;
	int ret;
	int size;

	init2();

	constraint.min = root.start;
	constraint.max = root.end;
	constraint.align = 1; // no alignment
	constraint.alignf = simple_align_resource;
	constraint.alignf_data = NULL;

	/* allocate 40 first */
	ret = find_resource(&root, &res[6], 40, &constraint);

	if (ret == 0)
	{
		printf("We find a size %d free slot under root at %lu-%lu aligned by 1\n",
			(int)resource_size(&res[6]), (long)res[6].start, (long)res[6].end);
		//request_resource_conflict(&root, &res[6]);
		dump(&root, 0);
	}
	else
		printf("We don't find a free slot under root for size 40\n");

	/* change the alignment and see the result */
	constraint.align = 4;
	ret = find_resource(&root, &res[6], 40, &constraint);

	if (ret == 0)
	{
		printf("We find a size %d free slot under root at %lu-%lu aligned by 4\n",
			(int)resource_size(&res[6]), (long)res[6].start, (long)res[6].end);
		//request_resource_conflict(&root, &res[6]);
		dump(&root, 0);
	}
	else
		printf("We don't find a free slot under root for size 40\n");

	/* allocate 90 first */
	constraint.max = 400; // but just look for it between 0-400
	ret = find_resource(&root, &res[7], 90, &constraint);

	if (ret == 0)
		printf("We find a size %d free slot under root at %lu-%lu aligned by 4\n",
			(int)resource_size(&res[7]), (long)res[7].start, (long)res[7].end);
	else
		printf("We don't find a free slot under root for size 90 aligned by 4 between 0-400\n");

	/* find biggest size aligned by 4*/
	constraint.max = root.end; // find it to the end of root
	size = resource_size(&root);
	while(size > 0)
	{
		ret = find_resource(&root, &res[8], size, &constraint);

		if (!ret)
			break;

		size -= 1;
	}

	if (ret == 0)
	{
		printf("We find a size %d free slot under root at %lu-%lu aligned by 4\n",
			(int)resource_size(&res[8]), (long)res[8].start, (long)res[8].end);

		dump(&root, 0);
	}
	else
		printf("No free space?\n");

	/* find biggest size aligned by 1*/
	constraint.max = root.end; // find it to the end of root
	constraint.align = 1;
	size = resource_size(&root);
	while(size > 0)
	{
		ret = find_resource(&root, &res[8], size, &constraint);

		if (!ret)
			break;

		size -= 1;
	}

	if (ret == 0)
	{
		printf("We find a size %d free slot under root at %lu-%lu aligned by 1\n",
			(int)resource_size(&res[8]), (long)res[8].start, (long)res[8].end);

		dump(&root, 0);
	}
	else
		printf("No free space?\n");

	return;
}

void adjust_resource_test()
{
	init2();

	adjust_resource(&res[1], 51, 60);
	printf("move res[1] %lu-%lu to %lu-%lu\n", 
			res[1].start, res[1].end, 51UL, (51+60)-1UL);
	dump(&root, 0);

	adjust_resource(&res[1], res[1].start, 100);
	printf("expand res[1] %lu-%lu to size %d\n",
			res[1].start, res[1].end, 100);
	dump(&root, 0);

	return;
}

/* some difference between the reallocate and adjust? 
 * the small difference between these two case
 * 1. since start address is 0, so __find_resouce() will get 
 *    51-140, which will conflict with its child
 * 2. then adjust the start from 70, then __find_resouce() will 
 *    get 70-159, this contains the child
 * */
void reallocate_resource_test()
{
	struct resource_constraint constraint;
	int ret;

	init2();

	constraint.min = 0;
	constraint.max = 250;
	constraint.align = 1; // no alignment
	constraint.alignf = simple_align_resource;
	constraint.alignf_data = NULL;

	/* expand the res? */
	ret = reallocate_resource(&root, &res[1], 90, &constraint);

	if (ret)
		printf("resource not allocated\n");
	else {
		printf("resource reallocated\n");
		dump(&root, 0);
	}

	constraint.min = 70;
	/* expand the res? */
	ret = reallocate_resource(&root, &res[1], 90, &constraint);

	if (ret)
		printf("resource not allocated\n");
	else {
		printf("resource reallocated\n");
		dump(&root, 0);
	}

	return;
}

void find_res_top_free_test()
{
	struct resource_constraint constraint;
	int ret;

	init2();

	/* actually the biggest free space under res[1] is 15 */
	printf("free size %d\n",(int)__find_res_top_free_size(&res[1]));

	/* find biggest size, by calling find_resource() */
	constraint.min = res[1].start;
	constraint.max = res[1].end;
	constraint.align = 1; // no alignment
	constraint.alignf = simple_align_resource;
	constraint.alignf_data = NULL;
	res[8].parent = NULL;
	int size = resource_size(&res[1]);

	while(size > 0)
	{
		ret = find_resource(&res[1], &res[8], size, &constraint);

		if (!ret)
			break;

		size -= 1;
	}

	if (ret == 0)
	{
		printf("We find a size %d free slot at %lu-%lu\n",
			(int)resource_size(&res[8]), (long)res[8].start, (long)res[8].end);

		dump(&root, 0);
	}
	else
		printf("No free space?\n");

}

/* this is not very well, since the function will break the tree structure */
void resource_extend_parents_top_test()
{
	init2();

	__resource_extend_parents_top(&res[4], 50, &root);
	dump(&root, 0);
	__resource_extend_parents_top(&res[4], 50, &root);
	dump(&root, 0);
}

void allocate_resource_test()
{
	struct resource res1, res2, res3;
	init2();

	res1.parent = NULL;
	res1.sibling = NULL;
	res1.child = NULL;
	printf("try to allocate a resource with size 10 under res[1] %ld-%ld\n",
			res[1].start, res[1].end);
	allocate_resource(&res[1], &res1, 10, res[1].start, res[1].end,
			1, NULL, NULL);
	dump(&root, 0);

        /* couldn't allocate resource since the boundary issue */
	res2.parent = NULL;
	res2.sibling = NULL;
	res2.child = NULL;
	printf("try to allocate a resource with size 10 under res[1] %ld-%ld\n",
			res[1].start, res[1].end);
	printf("but we must allocate from %ld-%ld, Fail\n", res[0].start, res[0].end);
	allocate_resource(&res[1], &res2, 10, res[0].start, res[0].end,
			1, NULL, NULL);

	dump(&root, 0);

        /*  change the boundary, allocate_resource() could find a suitable
	 *  aperture
	 */
	res3.parent = NULL;
	res3.sibling = NULL;
	res3.child = NULL;
	printf("try to allocate a resource with size 10 under res[1] %ld-%ld\n",
			res[1].start, res[1].end);
	printf("but we must allocate from %ld-%ld, Succeed\n", res[0].start, res[1].end);
	allocate_resource(&res[1], &res2, 10, res[0].start, res[1].end,
			1, NULL, NULL);
	dump(&root, 0);
}

void extend_res_test()
{
	insert_resource_test();

	/* extend res[4]=(105,140) first */
	printf("extend res[4] %d(%d-%d)\n",
		(int)resource_size(&res[4]), (int)res[4].start, (int)res[4].end);
	extend_res(&res[4], 20);
	dump(&root, 0);

	printf("extend res[4] %d(%d-%d)\n",
		(int)resource_size(&res[4]), (int)res[4].start, (int)res[4].end);
	extend_res(&res[4], 20);
	dump(&root, 0);
}

void request_region_test()
{
	init2();

	printf("\nThis test is used to show the difference between \n"
		"\trequest_resource_conflict()\n\t__request_region()\n\n\n");

	struct resource tmp;
	tmp.start = 105;
	tmp.end = 114;
	printf("1. try to use request_resource_conflict to request resource %lu-%lu\n", tmp.start, tmp.end);
	if (request_resource_conflict(&root, &tmp))
		printf("   failed to request resource %lu-%lu by request_resource_conflict\n", tmp.start, tmp.end);
	dump(&root, 0);

	printf("\n\n\n");
	/* this will allocate [105,114] deep in the tree */
	printf("2. try to use request_region to request resource %lu-%lu\n", tmp.start, tmp.end);
	__request_region(&root, tmp.start, resource_size(&tmp), "test", 0);
	dump(&root, 0);

	printf("\n---The difference is request_region() will go down the tree,\n"
		"   while request_resource_conflict() will just allocate at first level\n");
}

void reserve_region_with_split_test()
{
	init2();

	reserve_region_with_split(&root, 51, 300, "reserve_split");

	printf("after request 51-300 from root\n");
	dump(&root, 0);
}

void reserve_region_with_split_test1()
{
	init2();

	reserve_region_with_split1(&root, 51, 80, "reserve_split");

	printf("after request 51-80 from root\n");
	dump(&root, 0);
}

int p_res(struct resource *res, void *arg)
{
	printf("%lu-%lu  \n", res->start, res->end);
	return 0;
}

void iomem_resource_test()
{
	init3();
	walk_iomem_res_desc(0, 0, 0, 1000, NULL, p_res);
	walk_iomem_res_desc_rev(0, 0, 0, 1000, NULL, p_res);
}

int main()
{
	struct resource *p;
	iomem_resource_test();
	//allocate_resource_test();
	
	//for (p = iomem_resource.child; p;
	//	p = next_resource(p, false)) {
	//	printf("%lu-%lu  \n", p->start, p->end);
	//}
	//
	//for(p=prev_resource(NULL, false); p; p=prev_resource(p, false)) {
	//	printf("%lu-%lu  \n", p->start, p->end);
	//	if (p == iomem_resource.child) {
	//		break;
	//	}
	//}

	return 0;
}