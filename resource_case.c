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

    res[4].start = 90;
    res[4].end = 120;

    for(i = 0; i< 5; i++)
    {
	    __insert_resource(&root, &res[i]);
    }
    dump(&root, 0);

    printf("insert a large one which cover several res\n");
    res[5].start = 90;
    res[5].end = 260;
    __insert_resource(&root, &res[5]);
    dump(&root, 0);
    
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
                printf("%x\n", (unsigned int)&res[index]);
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

int main()
{
	init2();
	return 0;
}
