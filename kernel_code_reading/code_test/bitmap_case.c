/*
 * =====================================================================================
 *
 *       Filename:  bitmap_case.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/28/2013 11:25:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "types.h"
#include "bitmap.h"

DECLARE_BITMAP(bitmaps, 35);

int main()
{
	bitmap_zero(bitmaps, 35);
	return 0;
}
