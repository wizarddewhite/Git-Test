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


void realloc_test(struct resource *root, struct resource *res)
{
    int index;

    /*test cases for resoures*/
    dump(root);
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

    dump(root);

    return;
}

void relation_test()
{
	struct resource res1, res2;

	res1.start = 5;
	res1.end = 10;

	res2.start = 5;
	res2.end = 10;

	printf("res the same %d\n", res1_contains_res2(&res1, &res2));

	res2.start = 6;
	res2.end = 10;
	printf("res1 contain res2 %d\n", res1_contains_res2(&res1, &res2));

	res2.start = 5;
	res2.end = 11;
	printf("res1 contain res2 %d\n", res1_contains_res2(&res1, &res2));

	printf("res contain %d\n", resource_contains(&res1, &res2));

}
